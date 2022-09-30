/**
 * @file	definition_generator.cpp
 * @brief	Implements the MethodDefinitionPrinter.
 */

#include <experimental/iterator>

#include "definition_generator.hpp"

using namespace clang;
using namespace clang::ast_matchers;

namespace CppTinyRefactor
{

namespace detail
{

DefinitionPrinter::DefinitionPrinter(FunctionDeclarationLocation loc) :
    declaration_location{std::move(loc)}, printing_policy(LangOptions())
{
    printing_policy.SuppressTagKeyword = 1;
}

void DefinitionPrinter::run(const clang::ast_matchers::MatchFinder::MatchResult& result)
{
    auto node{result.Nodes.getNodeAs<clang::FunctionDecl>("function declaration")};
    if (node == nullptr or not node->getLocation().isValid())
        return;

    auto& source_manager{result.Context->getSourceManager()};
    auto source_location{node->getLocation()};
    auto presumed_loc{source_manager.getPresumedLoc(source_location)};

    auto matched_node_file_name{presumed_loc.getFilename()};
    auto matched_node_line_nr{presumed_loc.getLine()};

    if (matched_node_line_nr != declaration_location.line or matched_node_file_name != declaration_location.file)
        return;

    if (node->isImplicit())
        return;

    print_return_type_if_any(node);
    print_name(node);
    print_parameters(node);
    print_const_qualifier_if_has_one(node);

    output_stream << std::endl;
}

void DefinitionPrinter::print_return_type_if_any(const Decl* node)
{
    if (auto kind{node->getKind()}; kind != Decl::Kind::CXXConstructor and kind != Decl::Kind::CXXDestructor)
        output_stream << node->getReturnType().getAsString(printing_policy) << ' ';
}

void DefinitionPrinter::print_name(const Decl* node)
{
    output_stream << node->getQualifiedNameAsString();
}

void DefinitionPrinter::print_parameters(const Decl* node)
{
    std::vector<std::string> params_stringified;
    params_stringified.reserve(node->parameters().size());

    for (auto param : node->parameters())
    {
        auto type{param->getType().getAsString(printing_policy)};
        auto name{param->getNameAsString()};

        std::string param_as_string{type + ' ' + name};
        params_stringified.emplace_back(std::move(param_as_string));
    }

    output_stream << '(';
    std::copy(std::begin(params_stringified),
              std::end(params_stringified),
              std::experimental::make_ostream_joiner(output_stream, ", "));
    output_stream << ')';
}

void DefinitionPrinter::print_const_qualifier_if_has_one(const Decl* node)
{
    if (auto method_node{dynamic_cast<const clang::CXXMethodDecl*>(node)}; method_node != nullptr)
        if (method_node->isConst())
            output_stream << " const";
}

std::string DefinitionPrinter::get() const
{
    return output_stream.str();
}

} // namespace detail

DefinitionGenerator::DefinitionGenerator(FunctionDeclarationLocation loc) :
    printer{std::move(loc)}, matcher{functionDecl().bind("function declaration")}
{
    addMatcher(matcher, &printer);
}

std::string DefinitionGenerator::get() const
{
    return printer.get();
}

} // namespace CppTinyRefactor

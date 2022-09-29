/**
 * @file	definition_generator.cpp
 * @brief	Implements the MethodDefinitionPrinter.
 */

#include <experimental/iterator>
#include <iostream>

#include "definition_generator.hpp"

using namespace clang;
using namespace clang::ast_matchers;

namespace CppTinyRefactor
{

namespace detail
{

DefinitionPrinter::DefinitionPrinter(FileWithDeclaration f, LineWithDeclaration l) :
    file{std::move(f.value)}, line_number{l.value}, printing_policy(LangOptions())
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

    if (matched_node_line_nr != line_number or matched_node_file_name != file)
        return;

    if (node->isImplicit())
        return;

    print_return_type_if_any(node);
    print_name(node);
    print_parameters(node);
    print_const_qualifier_if_has_one(node);

    std::cout << std::endl;
}

void DefinitionPrinter::print_return_type_if_any(const Decl* node) const
{
    if (auto kind{node->getKind()}; kind != Decl::Kind::CXXConstructor and kind != Decl::Kind::CXXDestructor)
        std::cout << node->getReturnType().getAsString(printing_policy) << ' ';
}

void DefinitionPrinter::print_name(const Decl* node) const
{
    std::cout << node->getQualifiedNameAsString();
}

void DefinitionPrinter::print_parameters(const Decl* node) const
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

    std::cout << '(';
    std::copy(std::begin(params_stringified),
              std::end(params_stringified),
              std::experimental::make_ostream_joiner(std::cout, ", "));
    std::cout << ')';
}

void DefinitionPrinter::print_const_qualifier_if_has_one(const Decl* node) const
{
    if (auto method_node{dynamic_cast<const clang::CXXMethodDecl*>(node)}; method_node != nullptr)
        if (method_node->isConst())
            std::cout << " const";
}

} // namespace detail

DefinitionGenerator::DefinitionGenerator(FileWithDeclaration f, LineWithDeclaration l) :
    printer{f, l}, matcher{functionDecl().bind("function declaration")}
{
    addMatcher(matcher, &printer);
}

} // namespace CppTinyRefactor

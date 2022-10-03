/**
 * @file	definition_generator.cpp
 * @brief	Implements the MethodDefinitionPrinter.
 */

#include <experimental/iterator>

#include <clang/Lex/Lexer.h>

#include "definition_generator.hpp"

using namespace clang;
using namespace clang::ast_matchers;

namespace CppTinyRefactor
{

namespace detail
{

DefinitionPrinter::DefinitionPrinter(FunctionDeclarationLocation loc) :
    declaration_location{std::move(loc)}, printing_policy(lang_options)
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
    print_parameters(node, source_manager);
    print_const_qualifier_if_has_one(node);
    print_ref_qualifier_if_has_one(node);
    print_noexcept_qualifier_if_has_one(node, source_manager);

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

void DefinitionPrinter::print_parameters(const Decl* node, const clang::SourceManager& source_manager)
{
    output_stream << '(';

    if (auto param_source_range{node->getParametersSourceRange()}; param_source_range.isValid())
        output_stream << source_range_content_to_string(param_source_range, source_manager);

    output_stream << ')';
}

void DefinitionPrinter::print_const_qualifier_if_has_one(const Decl* node)
{
    if (auto method_node{dynamic_cast<const clang::CXXMethodDecl*>(node)}; method_node != nullptr)
        if (method_node->isConst())
            output_stream << " const";
}

void DefinitionPrinter::print_ref_qualifier_if_has_one(const Decl* node)
{
    auto method_node{dynamic_cast<const clang::CXXMethodDecl*>(node)};
    if (method_node == nullptr)
        return;

    auto ref_qualifier{method_node->getRefQualifier()};
    if (ref_qualifier == RefQualifierKind::RQ_LValue)
        output_stream << " &";
    else if (ref_qualifier == RefQualifierKind::RQ_RValue)
        output_stream << " &&";
}

void DefinitionPrinter::print_noexcept_qualifier_if_has_one(const Decl* node,
                                                            const clang::SourceManager& source_manager)
{
    auto source_range{node->getExceptionSpecSourceRange()};
    if (source_range.isInvalid())
        return;

    output_stream << " ";
    output_stream << source_range_content_to_string(source_range, source_manager);
}

std::string DefinitionPrinter::get() const
{
    return output_stream.str();
}

std::string DefinitionPrinter::source_range_content_to_string(const clang::SourceRange& source_range,
                                                              const clang::SourceManager& source_manager) const
{
    auto begin{source_range.getBegin()};
    auto temp_end{source_range.getEnd()};
    auto end{Lexer::getLocForEndOfToken(temp_end, 0, source_manager, lang_options)};

    return std::string(source_manager.getCharacterData(begin), source_manager.getCharacterData(end));
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

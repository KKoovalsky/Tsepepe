/**
 * @file	definition_generator.cpp
 * @brief	Implements the MethodDefinitionPrinter.
 */

#include <experimental/iterator>
#include <regex>

#include <clang/Lex/Lexer.h>

#include "definition_generator.hpp"

using namespace clang;
using namespace clang::ast_matchers;

namespace Tsepepe
{

namespace detail
{

DefinitionPrinter::DefinitionPrinter(FunctionDeclarationLocation loc) :
    declaration_location{std::move(loc)}, printing_policy(lang_options)
{
    printing_policy.adjustForCPlusPlus();
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

    print_return_type_if_any(node, source_manager);
    print_name(node);
    print_parameters(node, source_manager);
    print_const_qualifier_if_has_one(node);
    print_ref_qualifier_if_has_one(node);
    print_noexcept_qualifier_if_has_one(node, source_manager);
}

void DefinitionPrinter::print_return_type_if_any(const Decl* node, const clang::SourceManager& source_manager)
{
    if (has_explicit_return_type(node, source_manager))
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
    {
        auto parameters_as_is{source_range_content_to_string(param_source_range, source_manager)};
        std::regex default_param_regex{"\\s*=\\s*.*?(,|$)"};
        output_stream << std::regex_replace(parameters_as_is, default_param_regex, "$1");
    }

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

    std::string result_raw(source_manager.getCharacterData(begin), source_manager.getCharacterData(end));
    if (result_raw.empty())
        return result_raw;

    std::regex re{"\\s\\s+"};
    auto result{std::regex_replace(result_raw, re, " ")};

    auto is_space{[](unsigned char c) {
        return std::isspace(c);
    }};

    auto first_non_ws_it{std::find_if_not(result.begin(), result.end(), is_space)};
    result.erase(result.begin(), first_non_ws_it);
    auto last_non_ws_it{std::find_if_not(result.rbegin(), result.rend(), is_space)};
    result.erase(last_non_ws_it.base(), result.end());
    return result;
}

bool DefinitionPrinter::has_explicit_return_type(const Decl* node, const clang::SourceManager& source_manager) const
{
    auto return_type_as_written_in_code{
        source_range_content_to_string(node->getReturnTypeSourceRange(), source_manager)};
    return not return_type_as_written_in_code.empty();
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

} // namespace Tsepepe

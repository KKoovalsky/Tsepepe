/**
 * @file	finder.cpp
 * @brief	Implements the core of the suitable place in class finder.
 */
#include <clang/Lex/Lexer.h>

#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "common_types.hpp"
#include "file_grepper.hpp"
#include "libclang_utils/suitable_place_in_class_finder.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declaration
// --------------------------------------------------------------------------------------------------------------------
static const CXXMethodDecl* find_last_public_method_in_first_public_chain(const CXXRecordDecl*);
static std::optional<unsigned>
try_find_line_with_public_section(const std::string& cpp_file_content, const CXXRecordDecl*, const SourceManager&);
static unsigned find_line_with_opening_bracket(const CXXRecordDecl*, const SourceManager&);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::SuitablePublicMethodPlaceInCppFile Tsepepe::find_suitable_place_in_class_for_public_method(
    const std::string& cpp_file_content, const clang::CXXRecordDecl* node, const clang::SourceManager& source_manager)
{
    auto last_public_method_in_first_public_method_chain{find_last_public_method_in_first_public_chain(node)};
    if (last_public_method_in_first_public_method_chain != nullptr)
    {
        auto end_source_loc{last_public_method_in_first_public_method_chain->getEndLoc()};
        return {.line = source_manager.getPresumedLoc(end_source_loc).getLine()};
    } else if (auto maybe_first_public_section{
                   try_find_line_with_public_section(cpp_file_content, node, source_manager)};
               maybe_first_public_section)
    {
        return {.line = *maybe_first_public_section};
    } else
    {
        SuitablePublicMethodPlaceInCppFile result;
        result.line = find_line_with_opening_bracket(node, source_manager);
        result.is_public_section_needed = not node->isStruct();
        return result;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// Private implementations
// --------------------------------------------------------------------------------------------------------------------
static const CXXMethodDecl* find_last_public_method_in_first_public_chain(const CXXRecordDecl* record)
{
    auto is_public_explicit_method{[](const CXXMethodDecl* method) {
        return method->getAccess() == AccessSpecifier::AS_public and not method->isImplicit();
    }};

    auto is_not_public_or_is_public_implicit_method{[&](const CXXMethodDecl* method) {
        return not is_public_explicit_method(method);
    }};

    auto beg{record->method_begin()};
    auto end{record->method_end()};

    auto first_public_method_it{std::ranges::find_if(beg, end, is_public_explicit_method)};
    if (first_public_method_it == end)
        return nullptr;

    // Ideally, we could use find_if() to find the first non-public method, and then use std::prev(), but the method
    // range is a forward range.
    auto last_public_method_in_first_public_method_chain_it{first_public_method_it};
    for (auto it{first_public_method_it}; it != end; ++it)
    {
        if (is_not_public_or_is_public_implicit_method(*it))
            break;
        last_public_method_in_first_public_method_chain_it = it;
    }

    return *last_public_method_in_first_public_method_chain_it;
}

static std::optional<unsigned> try_find_line_with_public_section(const std::string& cpp_file_content,
                                                                 const CXXRecordDecl* record,
                                                                 const SourceManager& source_manager)
{
    auto get_class_body_begin_end_lines{[&]() {
        // FIXME: Use PresumedSourceRange instead of the clang::Lexer module.
        auto class_body_source_range{record->getSourceRange()};
        auto class_body_past_end_loc{class_body_source_range.getEnd()};
        auto class_body_end_loc{
            Lexer::getLocForEndOfToken(class_body_past_end_loc, 0, source_manager, record->getLangOpts())};
        auto class_body_begin_line{source_manager.getSpellingLineNumber(class_body_source_range.getBegin())};
        auto class_body_end_line{source_manager.getSpellingLineNumber(class_body_end_loc)};
        return std::make_pair(class_body_begin_line, class_body_end_line);
    }};

    auto get_line_nums_with_public_access_specifier_declarations{[&]() -> std::vector<unsigned> {
        return Tsepepe::grep_file(cpp_file_content, Tsepepe::RustRegexPattern{"public\\s*:"});
    }};

    auto class_body_line_range{get_class_body_begin_end_lines()};
    auto is_line_within_class_body{[&](unsigned line) {
        return line >= class_body_line_range.first and line <= class_body_line_range.second;
    }};

    auto lines_with_public_access_specifier_declarations{get_line_nums_with_public_access_specifier_declarations()};
    auto public_section_within_the_class_it{
        std::ranges::find_if(lines_with_public_access_specifier_declarations, is_line_within_class_body)};
    if (public_section_within_the_class_it != std::end(lines_with_public_access_specifier_declarations))
        return *public_section_within_the_class_it;
    else
        return {};
}

static unsigned find_line_with_opening_bracket(const CXXRecordDecl* record, const SourceManager& source_manager)
{
    auto location{record->getBeginLoc()};
    while (true)
    {
        auto maybe_token{Lexer::findNextToken(location, source_manager, record->getLangOpts())};
        if (not maybe_token)
            break;

        const auto& token{*maybe_token};
        location = token.getLocation();
        if (token.getKind() == tok::TokenKind::l_brace)
            break;
    }

    return source_manager.getSpellingLineNumber(location);
}

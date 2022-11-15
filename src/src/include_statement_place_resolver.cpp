/**
 * @file	include_statement_place_resolver.cpp
 * @brief	Implements the include resolver.
 */

#include "include_statement_place_resolver.hpp"

#include <optional>
#include <regex>
#include <string>

using namespace Tsepepe;

// --------------------------------------------------------------------------------------------------------------------
// Helper declaration
// --------------------------------------------------------------------------------------------------------------------
static std::optional<unsigned> find_last_local_include_statement_end_offset(const std::string& cpp_file_content);
static std::optional<unsigned> find_last_global_include_statement_end_offset(const std::string& cpp_file_content);

/** @brief Finds the include guard heading end offset.
 *  @returns  The offset of the beginning of the line just below the include guard heading.
 *
 *      #ifndef SOME_HEADER_HPP
 *      #define SOME_HEADER_HPP
 *      // <--- the function returns this file offset (the offset just below the heading of the include guard)
 *      // ...
 *
 *      #endif / * SOME_HEADER_HPP * /
 */
static std::optional<unsigned> find_include_guard_heading_end_offset(const std::string& cpp_file_content);
static std::optional<unsigned> find_pragma_once_end_offset(const std::string& cpp_file_content);
static std::optional<unsigned> find_header_comment_end_offset(const std::string& cpp_file_content);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::IncludeStatementPlace Tsepepe::resolve_include_statement_place(const std::string& cpp_file_content)
{
    if (auto maybe_last_local_include_end_offset{find_last_local_include_statement_end_offset(cpp_file_content)};
        maybe_last_local_include_end_offset)
        return {.offset = *maybe_last_local_include_end_offset};

    if (auto maybe_last_global_include_end_offset{find_last_global_include_statement_end_offset(cpp_file_content)};
        maybe_last_global_include_end_offset)
        return {.offset = *maybe_last_global_include_end_offset, .is_newline_needed = true};

    if (auto maybe_include_guard_heading_end_offset{find_include_guard_heading_end_offset(cpp_file_content)};
        maybe_include_guard_heading_end_offset)
        return {.offset = *maybe_include_guard_heading_end_offset, .is_newline_needed = true};

    if (auto maybe_pragma_once_end_offset{find_pragma_once_end_offset(cpp_file_content)}; maybe_pragma_once_end_offset)
        return {.offset = *maybe_pragma_once_end_offset, .is_newline_needed = true};

    if (auto maybe_header_comment_end_offset{find_header_comment_end_offset(cpp_file_content)};
        maybe_header_comment_end_offset)
        return {.offset = *maybe_header_comment_end_offset, .is_newline_needed = true};

    // When no '#include' statement found, nor the comment header, or include guard, then the place is at the top of the
    // file.
    return {.offset = 0};
}

// --------------------------------------------------------------------------------------------------------------------
// Helper definition
// --------------------------------------------------------------------------------------------------------------------
static std::optional<unsigned> find_last_local_include_statement_end_offset(const std::string& cpp_file_content)
{
    // FIXME: apply DRY "find_last_match()"
    std::regex re{"\n?[ \t\f\v]*\".*\"\\s+edulcni#"};

    auto beg{std::rbegin(cpp_file_content)};
    auto end{std::rend(cpp_file_content)};
    std::match_results<decltype(beg)> match;
    if (not std::regex_search(beg, end, match, re))
        return {};
    return std::distance(std::begin(cpp_file_content), match[0].first.base());
}

static std::optional<unsigned> find_last_global_include_statement_end_offset(const std::string& cpp_file_content)
{
    // FIXME: apply DRY "find_last_match()"
    std::regex re{"\n?[ \t\f\v]*>.*<\\s+edulcni#"};

    auto beg{std::rbegin(cpp_file_content)};
    auto end{std::rend(cpp_file_content)};
    std::match_results<decltype(beg)> match;
    if (not std::regex_search(beg, end, match, re))
        return {};
    return std::distance(std::begin(cpp_file_content), match[0].first.base());
}

static std::optional<unsigned> find_include_guard_heading_end_offset(const std::string& cpp_file_content)
{
    // FIXME: apply DRY "find_first_match()"
    std::regex re{"ifndef\\s+\\w+\n#define\\s+\\w+.*\n"};
    std::smatch match;
    auto is_match_found{std::regex_search(cpp_file_content, match, re)};
    if (not is_match_found)
        return {};

    auto end{match[0].second};
    return std::distance(std::begin(cpp_file_content), end);
}

static std::optional<unsigned> find_pragma_once_end_offset(const std::string& cpp_file_content)
{
    // FIXME: apply DRY "find_first_match()"
    std::regex re{"#pragma\\s+once\n?"};
    std::smatch match;
    auto is_match_found{std::regex_search(cpp_file_content, match, re)};
    if (not is_match_found)
        return {};

    auto end{match[0].second};
    return std::distance(std::begin(cpp_file_content), end);
}

static std::optional<unsigned> find_header_comment_end_offset(const std::string& cpp_file_content)
{
    std::regex re{"\\s*/\\*.*?\\*/[[:blank:]]*\n?", std::regex::extended};
    std::smatch match;
    auto is_match_found{std::regex_search(cpp_file_content, match, re)};
    if (not is_match_found)
        return {};

    // Is at the beginning of the file?
    if (match.position() == 0)
    {
        auto header_end_index{match[0].length()};
        return header_end_index;
    } else
        return {};
}

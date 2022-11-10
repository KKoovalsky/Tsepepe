/**
 * @file	include_statement_place_resolver.cpp
 * @brief	Implements the include resolver.
 */

#include "include_statement_place_resolver.hpp"

#include <optional>
#include <regex>
#include <string>

#include "file_grepper.hpp"

using namespace Tsepepe;

// --------------------------------------------------------------------------------------------------------------------
// Helper declaration
// --------------------------------------------------------------------------------------------------------------------
static std::optional<unsigned> find_line_with_last_local_include_statement(const std::string& cpp_file_content);
static std::optional<unsigned> find_line_with_last_global_include_statement(const std::string& cpp_file_content);

/** @brief Finds the include guard heading end line.
 *
 *      #ifndef SOME_HEADER_HPP
 *      #define SOME_HEADER_HPP <--- the function returns the line number of this line
 *
 *      // ...
 *
 *      #endif / * SOME_HEADER_HPP * /
 *
 */
static std::optional<unsigned> find_line_with_include_guard_heading_end(const std::string& cpp_file_content);
static std::optional<unsigned> find_line_with_pragma_once_statement(const std::string& cpp_file_content);
static unsigned get_last_line_of_header_comment(const std::string& cpp_file_content);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::Line Tsepepe::resolve_include_statement_place(const std::string& cpp_file_content)
{
    if (auto maybe_last_local_include_line{find_line_with_last_local_include_statement(cpp_file_content)};
        maybe_last_local_include_line)
        return *maybe_last_local_include_line;

    if (auto maybe_last_global_include_line{find_line_with_last_global_include_statement(cpp_file_content)};
        maybe_last_global_include_line)
        return *maybe_last_global_include_line;

    if (auto maybe_include_guard_line{find_line_with_include_guard_heading_end(cpp_file_content)};
        maybe_include_guard_line)
        return *maybe_include_guard_line;

    if (auto maybe_pragma_once_line{find_line_with_pragma_once_statement(cpp_file_content)}; maybe_pragma_once_line)
        return *maybe_pragma_once_line;

    return get_last_line_of_header_comment(cpp_file_content);
}

// --------------------------------------------------------------------------------------------------------------------
// Helper definition
// --------------------------------------------------------------------------------------------------------------------
static std::optional<unsigned> find_line_with_last_local_include_statement(const std::string& cpp_file_content)
{
    auto r{grep_file(cpp_file_content, RustRegexPattern{"#include\\s+\".*\""})};
    if (not r.empty())
        return r.back();
    return {};
}

static std::optional<unsigned> find_line_with_last_global_include_statement(const std::string& cpp_file_content)
{
    auto r{grep_file(cpp_file_content, RustRegexPattern{"#include\\s+<.*>"})};
    if (not r.empty())
        return r.back();
    return {};
}

static std::optional<unsigned> find_line_with_include_guard_heading_end(const std::string& cpp_file_content)
{
    auto r{
        grep_file(cpp_file_content, RustRegexPattern{"#ifndef\\s+(.*)\\b.*\n#define"}, {.enable_multiline_regex = 1})};
    if (not r.empty())
        return r.back();
    return {};
}

static std::optional<unsigned> find_line_with_pragma_once_statement(const std::string& cpp_file_content)
{
    auto r{grep_file(cpp_file_content, RustRegexPattern{"#pragma\\s+once"})};
    if (not r.empty())
        return r.back();
    return {};
}

static unsigned get_last_line_of_header_comment(const std::string& cpp_file_content)
{
    auto get_line_under_iterator{[](const std::string& s, std::string::const_iterator sought_it) {
        auto end{std::end(s)};
        auto it{std::find(std::begin(s), end, '\n')};
        unsigned line_number{1};
        while (it <= sought_it and it != end)
        {
            it = std::find(std::next(it), end, '\n');
            ++line_number;
        }
        return line_number;
    }};

    std::regex re{"\\s*/\\*.*?\\*/", std::regex::extended};
    std::smatch match;
    auto is_match_found{std::regex_search(cpp_file_content, match, re)};
    if (not is_match_found)
        return 1;

    // Is at the beginning of the file?
    if (match.position() == 0)
    {
        auto end{match[0].second};
        return get_line_under_iterator(cpp_file_content, --end);
    }

    return 1;
}

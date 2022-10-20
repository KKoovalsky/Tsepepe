/**
 * @file	clang_ast_utils.cpp
 * @brief	Implements the clang AST utilities.
 */

#include <clang/Lex/Lexer.h>

#include <regex>

#include "clang_ast_utils.hpp"

using namespace clang;

namespace Tsepepe::utils::clang_ast
{
std::string source_range_content_to_string(const clang::SourceRange& source_range,
                                           const clang::SourceManager& source_manager,
                                           const clang::LangOptions& lang_options)

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

} // namespace Tsepepe::utils::clang_ast

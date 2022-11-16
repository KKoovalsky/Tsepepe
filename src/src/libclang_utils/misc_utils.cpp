/**
 * @file	misc_utils.cpp
 * @brief	Implements miscellaneous clang AST utils.
 */

#include "libclang_utils/misc_utils.hpp"
#include "clang/Basic/LangOptions.h"

#include <iostream>
#include <regex>

#include <clang/Lex/Lexer.h>

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
void Tsepepe::dump_presumed_location(const clang::PresumedLoc& loc)
{
    std::cout << "Tsepepe::dump_presumed_location(): ";
    if (loc.isInvalid())
    {
        std::cout << "location invalid!" << std::endl;
        return;
    }

    std::cout << "File ID: " << loc.getFileID().getHashValue() << ", file name: " << loc.getFilename()
              << ", line: " << loc.getLine() << ", column: " << loc.getColumn()
              << ", has include location: " << loc.getIncludeLoc().isValid() << std::endl;
}

std::string Tsepepe::source_range_content_to_string(const clang::SourceRange& source_range,
                                                    const clang::SourceManager& source_manager,
                                                    const clang::LangOptions& lang_opts)
{
    auto is_space{[](unsigned char c) {
        return std::isspace(c);
    }};

    auto strip{[&](std::string& s) {
        auto first_non_ws_it{std::find_if_not(s.begin(), s.end(), is_space)};
        s.erase(s.begin(), first_non_ws_it);
        auto last_non_ws_it{std::find_if_not(s.rbegin(), s.rend(), is_space)};
        s.erase(last_non_ws_it.base(), s.end());
    }};

    auto begin{source_range.getBegin()};
    auto temp_end{source_range.getEnd()};
    auto end{Lexer::getLocForEndOfToken(temp_end, 0, source_manager, lang_opts)};

    std::string result_raw(source_manager.getCharacterData(begin), source_manager.getCharacterData(end));
    if (result_raw.empty())
        return result_raw;

    std::regex re{"\\s\\s+"};
    auto result{std::regex_replace(result_raw, re, " ")};

    strip(result);
    return result;
}

void Tsepepe::dump_token(const clang::Token& token)
{
    auto literal_data{token.getLiteralData()};
    std::string literal_data_value{literal_data == nullptr ? "<EMTPY>" : std::string(literal_data, token.getLength())};
    std::cout << "Tsepepe Clang::token: " << token.getName() << ", literal data: " << literal_data_value << std::endl;
}

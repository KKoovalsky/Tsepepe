/**
 * @file        misc_utils.hpp
 * @brief       Miscellaneous clang AST utils.
 */
#ifndef MISC_UTILS_HPP
#define MISC_UTILS_HPP

#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Token.h>

namespace Tsepepe
{

void dump_presumed_location(const clang::PresumedLoc&);

std::string
source_range_content_to_string(const clang::SourceRange&, const clang::SourceManager&, const clang::LangOptions&);

void dump_token(const clang::Token&);

}; // namespace Tsepepe

#endif /* MISC_UTILS_HPP */

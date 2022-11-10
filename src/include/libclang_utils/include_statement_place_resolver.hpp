/**
 * @file        include_statement_place_resolver.hpp
 * @brief
 */
#ifndef INCLUDE_STATEMENT_PLACE_RESOLVER_HPP
#define INCLUDE_STATEMENT_PLACE_RESOLVER_HPP

#include "clang/Frontend/ASTUnit.h"
#include <filesystem>
#include <string>

#include <clang/AST/AST.h>

namespace Tsepepe
{

using Line = unsigned;

Line resolve_include_statement_place(const std::filesystem::path& path_to_include_file,
                                     const std::string& cpp_file_content,
                                     const clang::ASTUnit& cpp_file_ast);

} // namespace Tsepepe

#endif /* INCLUDE_STATEMENT_PLACE_RESOLVER_HPP */

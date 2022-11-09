/**
 * @file        include_resolver.hpp
 * @brief       Resolves where and how to put an include to a C++ file.
 */
#ifndef INCLUDE_RESOLVER_HPP
#define INCLUDE_RESOLVER_HPP

#include "clang/Frontend/ASTUnit.h"
#include <filesystem>
#include <string>

#include <clang/AST/AST.h>

namespace Tsepepe
{

using Line = unsigned;

Line resolve_local_include(const std::filesystem::path& path_to_include_file,
                           const std::string& cpp_file_content,
                           const clang::ASTUnit& cpp_file_ast);

} // namespace Tsepepe

#endif /* INCLUDE_RESOLVER_HPP */

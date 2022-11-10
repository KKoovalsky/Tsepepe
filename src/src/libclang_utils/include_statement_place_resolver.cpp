/**
 * @file	include_statement_place_resolver.cpp
 * @brief	Implements the include resolver.
 */

#include "libclang_utils/include_statement_place_resolver.hpp"

Tsepepe::Line Tsepepe::resolve_include_statement_place(const std::filesystem::path& path_to_include_file,
                                                       const std::string& cpp_file_content,
                                                       const clang::ASTUnit& cpp_file_ast)
{
    return 1;
}

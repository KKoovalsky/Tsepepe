/**
 * @file        base_specifier_resolver.hpp
 * @brief       Resolves the base specifier within the base-clause.
 */
#ifndef BASE_SPECIFIER_RESOLVER_HPP
#define BASE_SPECIFIER_RESOLVER_HPP

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

#include "common_types.hpp"

namespace Tsepepe
{

CodeInsertionByOffset resolve_base_specifier(const std::string& cpp_file_content,
                                             const clang::CXXRecordDecl* deriving_class,
                                             const clang::CXXRecordDecl* base_class,
                                             const clang::SourceManager&);
}

#endif /* BASE_SPECIFIER_RESOLVER_HPP */

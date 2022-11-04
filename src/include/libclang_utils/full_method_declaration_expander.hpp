/**
 * @file        full_method_declaration_expander.hpp
 * @brief       Expands full method declaration.
 */
#ifndef FULL_METHOD_DECLARATION_EXPANDER_HPP
#define FULL_METHOD_DECLARATION_EXPANDER_HPP

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

std::string fully_expand_method_declaration(const clang::CXXMethodDecl*, const clang::SourceManager&);

}

#endif /* FULL_FUNCTION_DECLARATION_EXPANDER_HPP */

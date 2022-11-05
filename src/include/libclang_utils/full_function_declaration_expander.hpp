/**
 * @file        full_function_declaration_expander.hpp
 * @brief       Expands full function declaration.
 */
#ifndef FULL_FUNCTION_DECLARATION_EXPANDER_HPP
#define FULL_FUNCTION_DECLARATION_EXPANDER_HPP

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

std::string fully_expand_function_declaration(const clang::FunctionDecl*, const clang::SourceManager&);

}

#endif /* FULL_FUNCTION_DECLARATION_EXPANDER_HPP */

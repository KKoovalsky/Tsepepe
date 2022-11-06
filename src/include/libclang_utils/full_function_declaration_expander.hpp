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

struct FullFunctionDeclarationExpanderOptions
{
    // C++20 allows default bitfield values with the {} brackets.

    unsigned keep_attribute_specifiers : 1 {1};
};

std::string fully_expand_function_declaration(
    const clang::FunctionDecl*,
    const clang::SourceManager&,
    FullFunctionDeclarationExpanderOptions options = FullFunctionDeclarationExpanderOptions{});

} // namespace Tsepepe

#endif /* FULL_FUNCTION_DECLARATION_EXPANDER_HPP */

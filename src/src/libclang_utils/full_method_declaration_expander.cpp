/**
 * @file	full_method_declaration_expander.cpp
 * @brief	Implements the full expanding of the the method declaration.
 */
#include "libclang_utils/full_method_declaration_expander.hpp"
#include "clang/AST/DeclCXX.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/Basic/LangOptions.h>

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static std::string get_return_type(const clang::CXXMethodDecl*, const PrintingPolicy&);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::string Tsepepe::fully_expand_method_declaration(const clang::CXXMethodDecl* method)
{
    std::string result;
    result.reserve(120);

    LangOptions lang_opts;
    PrintingPolicy printing_policy{lang_opts};
    printing_policy.adjustForCPlusPlus();
    printing_policy.FullyQualifiedName = true;
    printing_policy.Indentation = 4;
    printing_policy.CleanUglifiedParameters = true;

    method->getDeclaredReturnType();
    auto return_type{method->getReturnType()};
    result.append(method->getReturnType().getAsString(printing_policy));
    result.append(method->getQualifiedNameAsString());

    return {};
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static std::string get_return_type(const clang::CXXMethodDecl*, const PrintingPolicy&)
{
    return {};
}

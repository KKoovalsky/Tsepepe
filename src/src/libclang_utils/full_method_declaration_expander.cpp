/**
 * @file	full_method_declaration_expander.cpp
 * @brief	Implements the full expanding of the the method declaration.
 */
#include "libclang_utils/full_method_declaration_expander.hpp"
#include "clang/AST/DeclCXX.h"
#include "clang/Basic/SourceLocation.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Lex/Lexer.h>

#include "libclang_utils/misc_utils.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static std::string get_return_type(const CXXMethodDecl*, const SourceManager&, const PrintingPolicy&);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::string Tsepepe::fully_expand_method_declaration(const CXXMethodDecl* method, const SourceManager& source_manager)
{
    std::string result;
    result.reserve(120);

    LangOptions lang_opts;
    PrintingPolicy printing_policy{lang_opts};
    printing_policy.adjustForCPlusPlus();
    printing_policy.FullyQualifiedName = true;
    printing_policy.Indentation = 4;
    printing_policy.CleanUglifiedParameters = true;

    result.append(get_return_type(method, source_manager, printing_policy));

    return result;
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
#include <iostream>
static std::string
get_return_type(const CXXMethodDecl* node, const SourceManager& source_manager, const PrintingPolicy& printing_policy)
{
    auto has_explicit_return_type{[&](const CXXMethodDecl* node) {
        auto return_type_as_written_in_code{Tsepepe::source_range_content_to_string(
            node->getReturnTypeSourceRange(), source_manager, node->getLangOpts())};
        std::cout << return_type_as_written_in_code << std::endl;
        return not return_type_as_written_in_code.empty();
    }};

    if (not has_explicit_return_type(node))
        return "";
    return node->getReturnType().getAsString(printing_policy);
}

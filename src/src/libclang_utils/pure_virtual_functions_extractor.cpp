/**
 * @file	pure_virtual_functions_extractor.cpp
 * @brief	Implements the pure virtual functions extractor.
 */

#include <regex>

#include "libclang_utils/full_function_declaration_expander.hpp"
#include "libclang_utils/pure_virtual_functions_extractor.hpp"

#include "scope_remover.hpp"

using namespace clang;
using namespace Tsepepe;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
OverrideDeclarations
Tsepepe::pure_virtual_functions_to_override_declarations(const clang::CXXRecordDecl* node,
                                                         std::string implementor_fully_qualified_name,
                                                         const clang::SourceManager& source_manager)
{
    OverrideDeclarations override_declarations;
    AllScopeRemover implementor_scopes_remover{FullyQualifiedName{implementor_fully_qualified_name}};

    auto append_override_declaration{[&](const CXXMethodDecl* method) {
        const auto& interface_name{method->getParent()->getQualifiedNameAsString()};

        auto declaration{Tsepepe::fully_expand_function_declaration(method, source_manager)};
        declaration = ScopeRemover{FullyQualifiedName{interface_name}}.remove_from(declaration);
        declaration = implementor_scopes_remover.remove_from(declaration);
        declaration.append(" override;");

        override_declarations.emplace_back(std::move(declaration));
    }};

    auto collect_override_declarations{[&](const clang::CXXRecordDecl* record) {
        for (auto method : record->methods())
            if (method->isPure())
                append_override_declaration(method);
    }};

    // The actual story begins here ...
    node->forallBases([&](const CXXRecordDecl* base) {
        collect_override_declarations(base);
        return true;
    });
    collect_override_declarations(node);
    return override_declarations;
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------

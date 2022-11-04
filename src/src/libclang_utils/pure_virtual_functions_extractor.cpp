/**
 * @file	pure_virtual_functions_extractor.cpp
 * @brief	Implements the pure virtual functions extractor.
 */

#include "libclang_utils/pure_virtual_functions_extractor.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::OverrideDeclarations
Tsepepe::pure_virtual_functions_to_override_declarations(const clang::CXXRecordDecl* node,
                                                         const clang::SourceManager& source_manager)
{
    OverrideDeclarations override_declarations;

    auto append_override_declaration{[&](const CXXMethodDecl* method) {
        // TODO
        // 1. Get the full method declaration.
        // 2. Shortify nesting from the base class.
        // 3. Shortify nesting from the namespace(s), the derived class is in.
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

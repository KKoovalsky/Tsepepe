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
        // auto declaration{Tsepepe::utils::clang_ast::source_range_content_to_string(
        //     method->getSourceRange(), source_manager, lang_options)};
        //
        // std::regex overrider{"virtual\\s+(.*)(\\s+=\\s+0)"};
        // auto override_declaration{std::regex_replace(declaration, overrider, "$1 override;")};
        //
        // function_override_declarations.emplace_back(std::move(override_declaration));
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

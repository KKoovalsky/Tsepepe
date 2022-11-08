/**
 * @file	pure_virtual_functions_extractor.cpp
 * @brief	Implements the pure virtual functions extractor.
 */

#include <regex>

#include "libclang_utils/full_function_declaration_expander.hpp"
#include "libclang_utils/pure_virtual_functions_extractor.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static std::string shortify(std::string shortified, std::string shorting);

/**
 * @brief Creates all the nesting scopes for the fully qualified name.
 *
 * For example, for a fully qualified name "SomeNamespace::SomeTopLevelClass::SomeNestedClass" will return a vector of
 * values:
 *              - SomeNamespace::SomeTopLevelClass::SomeNestedClass::
 *              - SomeNamespace::SomeTopLevelClass::
 *              - SomeNamespace::
 */
static std::vector<std::string> make_nesting_scopes(const std::string& fully_qualified_name);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::OverrideDeclarations
Tsepepe::pure_virtual_functions_to_override_declarations(const clang::CXXRecordDecl* node,
                                                         std::string implementor_fully_qualified_name,
                                                         const clang::SourceManager& source_manager)
{
    OverrideDeclarations override_declarations;
    auto implementor_nesting_scopes{make_nesting_scopes(implementor_fully_qualified_name)};

    auto append_override_declaration{[&](const CXXMethodDecl* method) {
        auto declaration{Tsepepe::fully_expand_function_declaration(method, source_manager)};

        auto interface_nesting_prefix{method->getParent()->getQualifiedNameAsString() + "::"};
        declaration = shortify(declaration, interface_nesting_prefix);

        auto implementer_nesting_prefix{node->getQualifiedNameAsString()};
        for (const auto& scope : implementor_nesting_scopes)
            declaration = shortify(declaration, scope);

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
static std::string shortify(std::string shortified, std::string shorting)
{
    return std::regex_replace(shortified, std::regex{shorting}, "");
}

static std::vector<std::string> make_nesting_scopes(const std::string& fully_qualified_name)
{
    std::vector<std::string> result;
    result.reserve(4);

    auto actual_begin{std::begin(fully_qualified_name)};

    auto it{std::rbegin(fully_qualified_name)};
    auto end{std::rend(fully_qualified_name)};
    while (it != end)
    {
        result.emplace_back(std::string(actual_begin, it.base()) + "::");
        it = std::find(it, end, ':');
        if (it == end)
            break;
        // Skip the "::"
        it += 2;
    }

    return result;
}

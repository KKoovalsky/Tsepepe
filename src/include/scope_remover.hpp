/**
 * @file        scope_remover.hpp
 * @brief       Removes scope prefixes from C++ code.
 */
#ifndef SCOPE_REMOVER_HPP
#define SCOPE_REMOVER_HPP

#include <string>

#include <NamedType/named_type.hpp>

namespace Tsepepe
{

// FIXME: Make it a struct which validates whether the format is: <One>::<Two>::<Three>::(...)
using FullyQualifiedName = fluent::NamedType<std::string, struct FullyQualifiedNameTag>;

/** @brief Replaces any scoped names starting with the fully qualified name, with a unscoped name.
 *
 *  If the cpp_code contains strings with names: "name + ::", they will be removed simply.
 *
 *  Examples:
 *
 *      auto result{ScopeRemove{FullyQualifiedName{"Namespace"}}.remove("Namespace::Struct Namespace::foo();")};
 *      ASSERT(result == "Struct foo();");
 *
 *      auto result{ScopeRemove{FullyQualifiedName{"Namespace::"}}.remove("Namespace::Struct Namespace::foo();")};
 *      ASSERT(result == "Struct foo();");
 *
 *  \note If the fully qualified name doesn't end with "::", then it will be automatically added.
 */
struct ScopeRemover
{
    explicit ScopeRemover(FullyQualifiedName);
    std::string remove_from(const std::string& cpp_code);

  private:
    std::string fully_qualified_name;
};

/** @brief Splits the fully qualified name into multiple scopes, and applies replacement for each of them.
 *
 *  Performs similar thing to the ScopeRemover, but applies replacement for each nested scope. Effectively, having a
 *  fully qualified name like: "Namespace::Scope1::Scope2", will run the ScopeRemover three times, for three
 *  different names:
 *
 *      - "Namespace::Scope1::Scope2::"
 *      - "Namespace::Scope1::"
 *      - "Namespace::"
 *
 *  in such order.
 *
 *  Example:
 *
 *      auto result{remove_scope(FullyQualifiedName{"Namespace::Scope"},
 *          "Namespace::Struct Namespace::Scope::foo();")};
 *      ASSERT(result == "Struct foo();");
 *
 *  \note If the fully qualified name doesn't end with "::", then it will be automatically added.
 */
struct AllScopeRemover
{
    explicit AllScopeRemover(FullyQualifiedName);
    std::string remove_from(std::string cpp_code);

  private:
    /**
     * @brief Creates all the nesting scopes for the fully qualified name.
     *
     * For example, for a fully qualified name "SomeNamespace::SomeTopLevelClass::SomeNestedClass" will return a vector
     * of values:
     *              - SomeNamespace::SomeTopLevelClass::SomeNestedClass::
     *              - SomeNamespace::SomeTopLevelClass::
     *              - SomeNamespace::
     */
    static std::vector<std::string> make_nesting_scopes(const std::string& fully_qualified_name);

    std::string fully_qualified_name;
    std::vector<std::string> nesting_scopes;
};

} // namespace Tsepepe

#endif /* SCOPE_REMOVER_HPP */

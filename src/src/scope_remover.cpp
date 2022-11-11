/**
 * @file	scope_remover.cpp
 * @brief	Implements the scope remover module.
 */

#include "scope_remover.hpp"

#include <regex>

Tsepepe::ScopeRemover::ScopeRemover(FullyQualifiedName name_alias) : fully_qualified_name{std::move(name_alias.get())}
{
    if (not fully_qualified_name.ends_with("::"))
        fully_qualified_name.append("::");
}

std::string Tsepepe::ScopeRemover::remove(const std::string& cpp_code)
{
    return std::regex_replace(cpp_code, std::regex{fully_qualified_name}, "");
}

Tsepepe::AllScopeRemover::AllScopeRemover(FullyQualifiedName name_alias) :
    nesting_scopes{make_nesting_scopes(name_alias.get())}
{
}

std::string Tsepepe::AllScopeRemover::remove(std::string cpp_code)
{
    for (const auto& scope : nesting_scopes)
        cpp_code = ScopeRemover{FullyQualifiedName{scope}}.remove(cpp_code);
    return cpp_code;
}

std::vector<std::string> Tsepepe::AllScopeRemover::make_nesting_scopes(const std::string& fully_qualified_name)
{
    std::vector<std::string> result;
    result.reserve(4);

    auto actual_begin{std::begin(fully_qualified_name)};

    auto it{std::rbegin(fully_qualified_name)};
    if (*it == ':')
        ++it;
    if (*it == ':')
        ++it;

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


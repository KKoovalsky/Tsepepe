/**
 * @file        symbol_finder.hpp
 * @brief       Finds symbols in a project.
 */
#ifndef SYMBOL_FINDER_HPP
#define SYMBOL_FINDER_HPP

#include <filesystem>
#include <vector>

#include <NamedType/named_type.hpp>

namespace Tsepepe
{

struct SymbolMatch
{
    std::filesystem::path path;
    unsigned line;
    unsigned column_begin, column_end;

    auto operator<=>(const SymbolMatch&) const = default;
};

using SymbolName = fluent::NamedType<std::string, struct SymbolNameTag>;

std::vector<SymbolMatch> find_symbol(const std::filesystem::path& root, SymbolName);

} // namespace Tsepepe

#endif /* SYMBOL_FINDER_HPP */

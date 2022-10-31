/**
 * @file        symbol_finder.hpp
 * @brief       Symbol finder interface.
 */
#ifndef SYMBOL_FINDER_HPP
#define SYMBOL_FINDER_HPP

#include <string>

#include "source_range.hpp"

namespace Tsepepe
{

struct SymbolFinder
{
    virtual SourceRange find(const std::string& symbol_name) = 0;
    virtual ~SymbolFinder() = default;
};

} // namespace Tsepepe

#endif /* SYMBOL_FINDER_HPP */

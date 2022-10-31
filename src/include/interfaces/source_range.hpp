/**
 * @file        source_range.hpp
 * @brief       Source range structure.
 */
#ifndef SOURCE_RANGE_HPP
#define SOURCE_RANGE_HPP

#include <string>

namespace Tsepepe
{

struct SourceRange
{
    std::string file;
    unsigned line_begin, line_end;
    unsigned column_begin, column_end;
};

} // namespace Tsepepe

#endif /* SOURCE_RANGE_HPP */

/**
 * @file        file_grepper.hpp
 * @brief       A file grepper.
 */
#ifndef FILE_GREPPER_HPP
#define FILE_GREPPER_HPP

#include <vector>

#include "common_types.hpp"

namespace Tsepepe
{

using Line = unsigned;
using FileGrepMatches = std::vector<Line>;

struct GrepOptions
{
    unsigned enable_multiline_regex : 1 {0};
};

FileGrepMatches grep_file(const std::string& file_content, RustRegexPattern, GrepOptions options = {});

} // namespace Tsepepe

#endif /* FILE_GREPPER_HPP */

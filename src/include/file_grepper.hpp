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

FileGrepMatches grep_file(const std::string& file_content, RustRegexPattern);

} // namespace Tsepepe

#endif /* FILE_GREPPER_HPP */

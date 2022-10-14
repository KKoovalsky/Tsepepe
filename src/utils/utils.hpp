/**
 * @file        utils.hpp
 * @brief       Declares the utilities.
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "regex_wrapper.hpp"

namespace Tsepepe::utils
{
RegexWrapper class_name_to_header_file_regex(std::string class_name);
};
#endif /* UTILS_HPP */

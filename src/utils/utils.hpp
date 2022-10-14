/**
 * @file        utils.hpp
 * @brief       Declares the utilities.
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include <regex>
#include <string_view>

namespace Tsepepe::utils
{
std::regex class_name_to_header_file_regex(std::string_view class_name);
};
#endif /* UTILS_HPP */

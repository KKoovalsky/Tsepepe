/**
 * @file	class_name_to_header_file_name_regex.cpp
 * @brief	Implements the class name to header file name regex utility.
 */

#include "utils.hpp"

#include <algorithm>
#include <regex>

Tsepepe::RegexWrapper Tsepepe::utils::class_name_to_header_file_regex(std::string class_name)
{
    if (class_name.empty())
        return {""};

    std::regex camel_case_match_regex{"([a-z0-9])([A-Z])"};
    auto class_name_snake_case{std::regex_replace(class_name, camel_case_match_regex, "$1_$2")};

    std::string class_name_snake_lower_case(class_name_snake_case.size(), '\0');
    std::ranges::transform(class_name_snake_case, std::begin(class_name_snake_lower_case), [](unsigned char c) {
        return std::tolower(c);
    });

    auto final_regex{std::regex_replace(class_name_snake_lower_case, std::regex{"_"}, "_*")};
    final_regex.append("\\.(h|hpp|hh|hxx)");
    return final_regex;
}

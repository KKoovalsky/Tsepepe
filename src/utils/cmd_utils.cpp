/**
 * @file	cmd_utils.cpp
 * @brief	Command Line utilities definition.
 */

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

#include "error.hpp"

namespace Tsepepe::utils::cmd
{

bool is_command_help_requested(int argc, const char** argv)
{
    auto is_help_option{[](const char* arg) {
        return std::strcmp(arg, "--help") == 0 or std::strcmp(arg, "-h") == 0;
    }};

    for (int i{1}; i < argc; ++i)
        if (is_help_option(argv[i]))
            return true;

    return false;
}

int parse_and_validate_number(const char* arg)
{
    std::string number_str{arg};
    if (not std::ranges::all_of(number_str, [](unsigned char c) { return std::isdigit(c); }))
        throw Tsepepe::Error{"Argument: " + number_str + " is not numeric!"};
    return std::stoi(number_str);
}

} // namespace Tsepepe::utils::cmd

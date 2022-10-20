/**
 * @file	cmd_utils.cpp
 * @brief	Command Line utilities definition.
 */

#include <cstring>

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

} // namespace Tsepepe::utils::cmd

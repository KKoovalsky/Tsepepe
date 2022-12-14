/**
 * @file        cmd_utils.hpp
 * @brief       Command line utilities for Tsepepe.
 */
#ifndef CMD_UTILS_HPP
#define CMD_UTILS_HPP

namespace Tsepepe::utils::cmd
{

bool is_command_help_requested(int argc, const char** argv);

int parse_and_validate_number(const char* arg);

} // namespace Tsepepe::utils::cmd
#endif /* CMD_UTILS_HPP */

/**
 * @file        cmd_parser.hpp
 * @brief       Parses the command arguments for the abstract class finder.
 */
#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <variant>

#include "input.hpp"

namespace Tsepepe::AbstractClassFinder
{

using ReturnCode = int;

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv);

} // namespace Tsepepe::AbstractClassFinder

#endif /* CMD_PARSER_HPP */

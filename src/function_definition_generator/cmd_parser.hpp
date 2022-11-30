/**
 * @file        cmd_parser.hpp
 * @brief       Command line parser for the function definition generator.
 */
#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <variant>

#include "input.hpp"

namespace Tsepepe::FunctionDefinitionGenerator
{

using ReturnCode = int;
std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv);

} // namespace Tsepepe::FunctionDefinitionGenerator

#endif /* CMD_PARSER_HPP */

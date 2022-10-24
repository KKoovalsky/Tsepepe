/**
 * @file        cmd_parser.hpp
 * @brief       Command line parser for the the suitable place in class finder.
 */
#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <variant>

#include "input.hpp"

namespace Tsepepe::SuitablePlaceInClassFinder
{

using ReturnCode = int;
std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv);

} // namespace Tsepepe::SuitablePlaceInClassFinder

#endif /* CMD_PARSER_HPP */

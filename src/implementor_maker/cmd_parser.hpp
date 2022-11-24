/**
 * @file        cmd_parser.hpp
 * @brief       Command line parser for the pure virtual functions extractor.
 */
#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <variant>

#include "input.hpp"

namespace Tsepepe::ImplementorMaker
{

using ReturnCode = int;
std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv);

} // namespace Tsepepe::ImplementorMaker

#endif /* CMD_PARSER_HPP */

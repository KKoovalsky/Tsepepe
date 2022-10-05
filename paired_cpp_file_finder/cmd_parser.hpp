/**
 * @file        cmd_parser.hpp
 * @brief       Command line parser for the paired C++ file finder.
 */
#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <string>
#include <variant>

namespace Tsepepe
{
namespace PairedCppFileFinder
{

struct Input
{
    std::string project_directory;
    std::string cpp_file;
};

using ReturnCode = int;

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv);

}; // namespace PairedCppFileFinder
} // namespace Tsepepe

#endif /* CMD_PARSER_HPP */

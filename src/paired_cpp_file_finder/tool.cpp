/**
 * @file	tool.cpp
 * @brief	Entry point for the paired C++ file finder tool.
 */
#include <iostream>

#include "cmd_parser.hpp"
#include "finder.hpp"

using namespace Tsepepe::PairedCppFileFinder;

int main(int argc, const char* argv[])
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    auto input{std::get<Input>(input_or_return_code)};
    auto matches{find(input)};
    if (matches.empty())
    {
        std::cerr << "ERROR: No paired C++ file found for: " << input.cpp_file
                  << ", under directory: " << input.project_directory << std::endl;
        return 1;
    }

    for (const auto& m : matches)
        std::cout << m.string() << std::endl;

    return 0;
}

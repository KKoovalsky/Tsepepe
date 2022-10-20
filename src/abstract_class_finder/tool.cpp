/**
 * @file	tool.cpp
 * @brief	Entry point for the abstract class finder tool.
 */

#include <iostream>

#include "cmd_parser.hpp"
#include "finder.hpp"

using namespace Tsepepe::AbstractClassFinder;

int main(int argc, const char* argv[])
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    const auto& input{std::get<Input>(input_or_return_code)};
    auto matching_headers{find(input)};
    if (matching_headers.empty())
    {
        std::cerr << "ERROR! No abstract class named " << input.class_name << " under root: " << input.project_root
                  << std::endl;
        return 1;
    }

    for (auto header_path : matching_headers)
        std::cout << header_path.string() << std::endl;

    return 0;
}

/**
 * @file	tool.cpp
 * @brief	The entry point for the suitable place in class finder.
 */
#include <iostream>

#include "cmd_parser.hpp"
#include "finder.hpp"

using namespace Tsepepe::SuitablePlaceInClassFinder;

int main(int argc, const char** argv)
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    const auto& input{std::get<Input>(input_or_return_code)};
    auto maybe_suitable_line{find(input)};
    if (!maybe_suitable_line)
    {
        std::cerr << "ERROR: no suitable place in class found!" << std::endl;
        return 1;
    }

    std::cout << *maybe_suitable_line << std::endl;
    return 0;
}

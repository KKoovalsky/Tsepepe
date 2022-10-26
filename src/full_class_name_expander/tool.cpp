/**
 * @file	tool.cpp
 * @brief	Main entry point for the pure virtual functions extractor.
 */

#include <iostream>

#include "cmd_parser.hpp"
#include "expander.hpp"
#include "input.hpp"

using namespace Tsepepe::FullClassNameExpander;

int main(int argc, const char* argv[])
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    const auto& input{std::get<Input>(input_or_return_code)};
    auto full_class_name{expand(input)};
    if (full_class_name.empty())
    {
        std::cerr << "ERROR: Class name could not be expanded!" << std::endl;
        return 1;
    }

    std::cout << full_class_name;
    return 0;
}

/**
 * @file	tool.cpp
 * @brief	Main entry point for the pure virtual functions extractor.
 */

#include <iostream>

#include "base_error.hpp"
#include "cmd_parser.hpp"
#include "input.hpp"

#include "implement_interface_code_action.hpp"

using namespace Tsepepe::ImplementorMaker;

int main(int argc, const char* argv[])
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    auto input{std::move(std::get<Input>(input_or_return_code))};

    try
    {
        auto result{Tsepepe::ImplementIntefaceCodeActionLibclangBased{std::move(input.compilation_database_ptr)}.apply(
            std::move(input.parameters))};
        std::cout << result;
        return 0;
    } catch (const Tsepepe::BaseError& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

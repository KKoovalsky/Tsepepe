/**
 * @file	tool.cpp
 * @brief	The main app entry for the function definition generator.
 */
#include <iostream>

#include "cmd_parser.hpp"

#include "base_error.hpp"
#include "generate_function_definitions_code_action.hpp"

using namespace Tsepepe;
using namespace Tsepepe::FunctionDefinitionGenerator;

int main(int argc, const char** argv)
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    auto input{std::move(std::get<Input>(input_or_return_code))};

    try
    {
        auto result{GenerateFunctionDefinitionsCodeActionLibclangBased{std::move(input.compilation_database_ptr)}.apply(
            std::move(input.parameters))};

        if (result.empty())
        {
            std::cerr << "ERROR: No valid declaration found!\n" << std::endl;
            return 1;
        }

        std::cout << result;
        return 0;
    } catch (const Tsepepe::BaseError& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

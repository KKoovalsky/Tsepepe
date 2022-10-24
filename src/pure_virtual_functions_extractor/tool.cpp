/**
 * @file	tool.cpp
 * @brief	Main entry point for the pure virtual functions extractor.
 */

#include <iostream>

#include "cmd_parser.hpp"
#include "extractor.hpp"
#include "input.hpp"

using namespace Tsepepe::PureVirtualFunctionsExtractor;

int main(int argc, const char* argv[])
{
    auto input_or_return_code{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input_or_return_code))
        return std::get<ReturnCode>(input_or_return_code);

    const auto& input{std::get<Input>(input_or_return_code)};
    auto override_declarations{extract(input)};
    if (override_declarations.empty())
    {
        std::cerr << "ERROR: no pure virtual functions found!" << std::endl;
        return 1;
    }

    for (auto decl : override_declarations)
        std::cout << decl << std::endl;

    return 0;
}

/**
 * @file	tool.cpp
 * @brief	Entry point for the abstract class finder tool.
 */

#include "cmd_parser.hpp"
#include "finder.hpp"

using namespace Tsepepe::AbstractClassFinder;

int main(int argc, const char* argv[])
{
    auto input{parse_cmd(argc, argv)};
    if (std::holds_alternative<ReturnCode>(input))
        return std::get<ReturnCode>(input);

    return find(std::get<Input>(input));
    // Tsepepe::AbstractClassFinder::Finder finder{std::move(input)};
    //
    // ClangTool tool{*compilation_database_ptr, {header_file}};
    //
    // auto return_code{tool.run(newFrontendActionFactory(&finder).get())};
    // std::cout << "Match found?: " << validator.is_match_found() << std::endl;
    // return return_code;
}

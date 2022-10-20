/**
 * @file	tool.cpp
 * @brief	Main entry point for the pure virtual functions extractor.
 */

#include <string>

#include <clang/Tooling/CompilationDatabase.h>

#include <iostream>

#include "extractor.hpp"
#include "input.hpp"

using namespace Tsepepe::PureVirtualFunctionsExtractor;

int main(int argc, const char* argv[])
{
    std::string err;
    Input input{.compilation_database_ptr = clang::tooling::CompilationDatabase::loadFromDirectory(argv[1], err),
                .header_file = argv[2],
                .class_name{argv[3]}};

    auto override_declarations{extract(input)};
    for (auto decl : override_declarations)
    {
        std::cout << decl << std::endl;
    }

    return 0;
}

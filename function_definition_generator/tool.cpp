/**
 * @file	tool.cpp
 * @brief	The main app entry for the function definition generator.
 */
#include <filesystem>
#include <iostream>

#include <clang/Basic/Diagnostic.h>
#include <clang/Tooling/Tooling.h>

#include "cmd_parser.hpp"
#include "definition_generator.hpp"

using namespace clang;
using namespace clang::tooling;

namespace fs = std::filesystem;

using namespace Tsepepe;

int main(int argc, const char** argv)
{
    DefinitionGeneratorCmdParser cmd_parser{argc, argv};
    if (!cmd_parser)
    {
        cmd_parser.dump_streams();
        return cmd_parser.get_return_code();
    }

    auto declaration_location{cmd_parser.get_function_declaration_location()};

    // Replace potentially relative dir to a normalized absolute one.
    auto file{fs::absolute(declaration_location.file).lexically_normal().string()};
    declaration_location.file = file;

    ClangTool tool(cmd_parser.get_compilation_database(), declaration_location.file);

    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    DefinitionGenerator generator{declaration_location};
    auto return_code{tool.run(newFrontendActionFactory(&generator).get())};
    auto definition{generator.get()};
    if (definition.empty())
    {
        std::cerr << "ERROR: No valid declaration found!\n" << std::endl;
        return 1;
    } else
    {
        std::cout << generator.get();
    }
    return return_code;
}

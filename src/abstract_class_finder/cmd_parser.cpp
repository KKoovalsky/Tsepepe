/**
 * @file	cmd_parser.cpp
 * @brief	Implements the command line parsing for the abstract class finder.
 */

#include <iostream>

#include "clang_ast_utils.hpp"
#include "cmd_utils.hpp"
#include "error.hpp"
#include "filesystem_utils.hpp"

#include "cmd_parser.hpp"

using namespace Tsepepe::AbstractClassFinder;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::variant<Input, ReturnCode> Tsepepe::AbstractClassFinder::parse_cmd(int argc, const char** argv)
{
    if (Tsepepe::utils::cmd::is_command_help_requested(argc, argv))
    {
        print_usage(argc, argv);
        return ReturnCode{0};
    }

    if (argc != 4)
    {
        print_usage(argc, argv);
        return ReturnCode{1};
    }

    try
    {
        Input result;
        result.compilation_database_ptr = Tsepepe::utils::clang_ast::parse_compilation_database(argv[1]);
        result.project_root = Tsepepe::utils::fs::parse_and_validate_path(argv[2]);
        result.class_name = argv[3];
        return result;
    } catch (const Tsepepe::Error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return ReturnCode{1};
    }
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv)
{
    auto program_path{argv[0]};
    std::cout << "USAGE:\n\t" << program_path << " COMP_DB_DIR PROJECT_ROOT_DIR CLASS_NAME\n\n";
    std::cout << "DESCRIPTION:\n\tTries to find an abstract class with the name CLASS_NAME, under the root directory "
                 "PROJECT_ROOT_DIR.\n\t";
    std::cout << "Requires compilation database (compile_commands.json) put in COMP_DB_DIR directory.\n\t";
    std::cout << "On success, prints out the header file where the abstract class is located.\n\n";
    std::cout << "NOTE:\n\tripgrep tool is used to find the abstract class, thus the .gitignore patterns are used to"
                 " skip git-ignored directories.\n"
              << std::endl;
}


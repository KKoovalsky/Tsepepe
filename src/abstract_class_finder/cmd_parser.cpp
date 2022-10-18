/**
 * @file	cmd_parser.cpp
 * @brief	Implements the command line parsing for the abstract class finder.
 */

#include <iostream>

#include "utils.hpp"

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
    if (Tsepepe::utils::is_command_help_requested(argc, argv))
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
        result.compilation_database_ptr = Tsepepe::utils::parse_compilation_database(argv[1]);
        result.project_root = Tsepepe::utils::parse_and_validate_path(argv[2]);
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
}


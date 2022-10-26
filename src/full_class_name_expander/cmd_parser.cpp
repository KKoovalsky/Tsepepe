/**
 * @file	cmd_parser.cpp
 * @brief	Implements command parsing for pure virtual functions extractor.
 */
#include <iostream>

#include "cmd_parser.hpp"

#include "clang_ast_utils.hpp"
#include "cmd_utils.hpp"
#include "error.hpp"
#include "filesystem_utils.hpp"

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
namespace Tsepepe::FullClassNameExpander
{

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv)
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
        result.header_file = Tsepepe::utils::fs::parse_and_validate_path(argv[2]);
        result.class_name = argv[3];
        return result;
    } catch (const Tsepepe::Error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return ReturnCode{1};
    }
}

} // namespace Tsepepe::FullClassNameExpander

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv)
{
    auto program_path{argv[0]};
    std::cout << "USAGE:\n\t" << program_path << " COMP_DB_DIR HEADER_FILE CLASS_NAME\n\n";
    std::cout << "DESCRIPTION:\n\tPrints out the full name of the class. Takes into account nested classes."
                 "\n\tThe input file HEADER_FILE must contain a class called CLASS_NAME."
                 "\n\tRequires compilation database (compile_commands.json) put in the COMP_DB_DIR directory."
                 "\n\n"
                 "EXAMPLE:"
                 "\n\tHaving a header file \"printer.hpp\" with content:"
                 "\n\n\t\t// ..."
                 "\n\t\tnamespace Interface {"
                 "\n\t\tclass Printer {"
                 "\n\t\t\tstruct Record {"
                 "\n\t\t\t// ..."
                 "\n\n\tWhen the tool is called like that:"
                 "\n\n\t\ttsepepe_full_class_name_expander path/to/dir/with/comp/db printer.hpp Record"
                 "\n\n\tThe output will be:"
                 "\n\n\t\tInterface::Printer::Record"
                 "\n"
              << std::endl;
}

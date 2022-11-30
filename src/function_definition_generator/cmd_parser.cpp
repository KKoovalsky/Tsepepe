/**
 * @file	cmd_parser.cpp
 * @brief	Implements command parsing for the function definition generator.
 */
#include <filesystem>
#include <iostream>

#include "cmd_parser.hpp"

#include "clang_ast_utils.hpp"
#include "cmd_utils.hpp"
#include "error.hpp"
#include "filesystem_utils.hpp"

namespace fs = std::filesystem;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv);
static fs::path parse_and_validate_temporary_file_path(const char*);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
namespace Tsepepe::FunctionDefinitionGenerator
{

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv)
{
    if (Tsepepe::utils::cmd::is_command_help_requested(argc, argv))
    {
        print_usage(argc, argv);
        return ReturnCode{0};
    }

    if (argc != 5 and argc != 6)
    {
        std::cerr << "ERROR: Wrong number of arguments provided!\n" << std::endl;
        print_usage(argc, argv);
        return ReturnCode{1};
    }

    try
    {
        Input result;
        result.compilation_database_ptr = Tsepepe::utils::clang_ast::parse_compilation_database(argv[1]);

        GenerateFunctionDefinitionsCodeActionParameters params;
        params.source_file_path = parse_and_validate_temporary_file_path(argv[2]);
        params.source_file_content = argv[3];
        params.selected_line_begin = Tsepepe::utils::cmd::parse_and_validate_number(argv[4]);
        if (argc == 6)
            params.selected_line_end = Tsepepe::utils::cmd::parse_and_validate_number(argv[5]);
        else
            params.selected_line_end = params.selected_line_begin;

        result.parameters = std::move(params);
        return result;
    } catch (const Tsepepe::Error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return ReturnCode{1};
    }
}

} // namespace Tsepepe::FunctionDefinitionGenerator

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv)
{
    auto program_path{argv[0]};
    std::cout << "USAGE:\n\t" << program_path
              << " COMP_DB_DIR"
                 " SOURCE_FILE_PATH"
                 " SOURCE_FILE_CONTENT"
                 " CURSOR_POSITION_LINE_BEGIN"
                 " [CURSOR_POSITION_LINE_END]"
                 " \n\n";
    std::cout << "DESCRIPTION:"
                 "\n\tTakes the entire source file (SOURCE_FILE_CONTENT) and generates function definitions"
                 "\n\tfor each function declaration which can be found in the range of lines"
                 "\n\t<CURSOR_POSITION_LINE_BEGIN; CURSOR_POSITION_LINE_END>. The CURSOR_POSITION_LINE_END is"
                 "\n\toptional. When it is not set, then the definition for a declaration found at"
                 "\n\tCURSOR_POSITION_LINE_BEGIN will only be generated."
                 "\n\n\tThe path to the source file (SOURCE_FILE_PATH) is needed to properly resolve the includes,"
                 "\n\tthat might be found within the source file. The content of the file must be supplied as is"
                 "\n\twith the SOURCE_FILE_CONTENT parameter. Ideally, the newline separator should be '\\n 'character."
                 "\n\n\tWe need the path to the directory containing compile_commands.json as well,"
                 "\n\twhich shall be supplied with COMP_DB_DIR parameter."
                 "\n\n"
              << std::endl;
}

static fs::path parse_and_validate_temporary_file_path(const char* path_raw)
{
    fs::path path{path_raw};
    fs::path parent_path{path.has_filename() ? path.parent_path() : path};
    if (not fs::exists(parent_path))
        throw Tsepepe::Error{"Parent path: " + parent_path.string() + " of the path: " + path.string()
                             + " does not exist!"};
    return path;
}

/**
 * @file	cmd_parser.cpp
 * @brief	Implements command parsing for pure virtual functions extractor.
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
namespace Tsepepe::ImplementorMaker
{

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv)
{
    if (Tsepepe::utils::cmd::is_command_help_requested(argc, argv))
    {
        print_usage(argc, argv);
        return ReturnCode{0};
    }

    if (argc != 7)
    {
        std::cerr << "ERROR: Wrong number of arguments provided!\n" << std::endl;
        print_usage(argc, argv);
        return ReturnCode{1};
    }

    try
    {
        Input result;
        result.compilation_database_ptr = Tsepepe::utils::clang_ast::parse_compilation_database(argv[1]);

        ImplementInterfaceCodeActionParameters params;
        params.root_directory = Tsepepe::utils::fs::parse_and_validate_path(argv[2]);
        params.source_file_path = parse_and_validate_temporary_file_path(argv[3]);
        params.source_file_content = argv[4];
        params.inteface_name = argv[5];
        params.cursor_position_line = Tsepepe::utils::cmd::parse_and_validate_number(argv[6]);

        result.parameters = std::move(params);
        return result;
    } catch (const Tsepepe::Error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return ReturnCode{1};
    }
}

} // namespace Tsepepe::ImplementorMaker

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv)
{
    auto program_path{argv[0]};
    std::cout << "USAGE:\n\t" << program_path
              << " COMP_DB_DIR"
                 " ROOT_DIRECTORY"
                 " SOURCE_FILE_PATH"
                 " SOURCE_FILE_CONTENT"
                 " INTERFACE_NAME"
                 " CURSOR_POSITION_LINE"
                 " \n\n";
    std::cout << "DESCRIPTION:"
                 "\n\tTakes the entire source file (SOURCE_FILE_CONTENT) with a class definition,"
                 "\n\twhich can be found at line CURSOR_POSITION_LINE."
                 "\n\n\tThe line number must be within the class (or struct) source range."
                 "\n\n\tThe path to the source file (SOURCE_FILE_PATH) is needed to properly resolve the includes,"
                 "\n\tthat might be found within the source file."
                 "\n\n\tThe ROOT_DIRECTORY is needed to find the INTERFACE_NAME recursively,"
                 "\n\twithin the project's C++ source files."
                 "\n\n\tThe INTERFACE_NAME may be a raw name, thus, when it is a nested interface,"
                 "\n\twithin another class or a namespace, then the bare name, without the parent scope,"
                 "\n\tmust be specified (e.g. for 'Namespace::Interface' simply pass 'Interface')."
                 "\n\n\tWe need the path to the directory containing compile_commands.json as well,"
                 "\n\twhich shall be supplied with COMP_DB_DIR parameter."
                 "\n\n"
                 "EXAMPLE:"
                 "\n\tHaving a project under path <PROJECT_ROOT>, and an interface defined within a file "
                 "'src/include/yolo_interface.hpp', with content:"
                 "\n\n\t\tnamespace Tsepepe"
                 "\n\t\t{"
                 "\n\t\tstruct YoloInterface"
                 "\n\t\t{"
                 "\n\t\t    virtual void do_stuff() = 0;"
                 "\n\t\t    virtual ~YoloInterface() = default;"
                 "\n\t\t};"
                 "\n\t\t} // namespace Tsepepe"
                 "\n\t\t"
                 "\n\n\tWhen the tool is called like that:"
                 "\n\n\t\ttsepepe_implementor_maker"
                 "\n\t\t\t<PROJECT_ROOT>/build               # The path to the directory with the compile_commands.json"
                 "\n\t\t\t<PROJECT_ROOT>                     # The project root directory"
                 "\n\t\t\t<PROJECT_ROOT>/src/implementor.hpp # Path to the file with the potential implementor"
                 "\n\t\t\t'struct Implementor { };'          # The source file content"
                 "\n\t\t\tYoloInterface                      # The inteface name."
                 "\n\t\t\t1                                  # The first line contains the Implementor definition."
                 "\n\n\tThe output will be:"
                 "\n\n\t\t#include \"yolo_interface.hpp\""
                 "\n\t\tstruct Implementor : Tsepepe::YoloInterface {"
                 "\n\t\t    void do_stuff() override;"
                 "\n\t\t};"
                 "\n"
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

/**
 * @file	cmd_parser.cpp
 * @brief	Defines the DefinitionGeneratorCmdParser.
 */

#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "cmd_parser.hpp"

namespace fs = std::filesystem;

namespace Tsepepe
{

DefinitionGeneratorCmdParser::DefinitionGeneratorCmdParser(int argc, const char** argv) : status{validate(argc, argv)}
{
    if (not status.can_continue)
        return;

    declaration_location =
        FunctionDeclarationLocation{.file = argv[2], .line = static_cast<unsigned>(std::stoul(argv[3]))};

    std::string err;
    compilation_database_ptr = clang::tooling::CompilationDatabase::loadFromDirectory(argv[1], err);
    if (!compilation_database_ptr)
    {
        status = {.can_continue = false, .return_code = 1};
        err_stream << "ERROR: clang::tooling::CompilationDatabase" << err << std::endl;
    }
}

DefinitionGeneratorCmdParser::Status DefinitionGeneratorCmdParser::validate(int argc, const char** argv)
{
    if (is_help_requested(argc, argv))
    {
        print_usage(argv[0]);
        return {.can_continue = false, .return_code = 0};
    }

    try
    {
        validate_argument_number(argc, argv);
        validate_compilation_database_path(argv[1]);
        validate_source_file_exists(argv[2]);
        validate_line_number_is_numeric(argv[3]);
    } catch (const ArgumentError& e)
    {
        err_stream << e.what() << "\n" << std::endl;
        print_usage(argv[0]);
        return {.can_continue = false, .return_code = 1};
    } catch (const ValueError& e)
    {
        err_stream << e.what() << "\n" << std::endl;
        return {.can_continue = false, .return_code = 1};
    }

    return {.can_continue = true, .return_code = 0};
}

void DefinitionGeneratorCmdParser::validate_argument_number(int argc, const char**)
{
    if (argc != 4)
    {
        throw ArgumentError{"ERROR: wrong number of arguments specified!"};
    }
}

void DefinitionGeneratorCmdParser::validate_compilation_database_path(const char* path_to_compile_db)
{
    if (not fs::exists(path_to_compile_db))
    {
        std::string msg;
        msg.append("ERROR: Path to the compilation database: ");
        msg.append(path_to_compile_db);
        msg.append(" does not exist!");
        throw ValueError{std::move(msg)};
    }

    auto comp_db_path{fs::path(path_to_compile_db).append("compile_commands.json")};
    if (not fs::exists(comp_db_path))
    {
        std::string msg;
        msg.append("ERROR: Directory: ");
        msg.append(path_to_compile_db);
        msg.append(" does not contain a compilation database (compile_commands.json)!");
        throw ValueError{std::move(msg)};
    }
}

void DefinitionGeneratorCmdParser::validate_source_file_exists(const char* path_to_source_file)
{
    if (not fs::exists(path_to_source_file))
    {
        std::string msg;
        msg.append("ERROR: Source file: ");
        msg.append(path_to_source_file);
        msg.append(" does not exist!");
        throw ValueError{std::move(msg)};
    }
}

void DefinitionGeneratorCmdParser::validate_line_number_is_numeric(const char* line_number_str)
{
    auto ptr{line_number_str};
    while (*ptr != '\0')
    {
        if (not std::isdigit(*ptr++))
        {
            std::string msg;
            throw ValueError{"ERROR: Line number must be a positive number!"};
        }
    }
}

bool DefinitionGeneratorCmdParser::is_help_requested(int argc, const char** argv)
{
    for (int i{1}; i < argc; ++i)
        if (std::strcmp(argv[i], "--help") == 0 or std::strcmp(argv[i], "-h") == 0)
            return true;
    return false;
}

void DefinitionGeneratorCmdParser::print_usage(const char* program_path)
{
    out_stream << "USAGE:\n\t" << program_path << " COMP_DB_DIR SOURCE_FILE DECL_LINE\n\n";
    out_stream
        << "DESCIPTION:\n\tGenerates function definition from a declaration found in the SOURCE_FILE at line DECL_LINE."
        << std::endl;
    out_stream << "\tNeeds the compilation database that can be found under the COMP_DB_DIR directory." << std::endl;
}

DefinitionGeneratorCmdParser::operator bool() const noexcept
{
    return status.can_continue;
}

int DefinitionGeneratorCmdParser::get_return_code() const noexcept
{
    return status.return_code;
}

const clang::tooling::CompilationDatabase& DefinitionGeneratorCmdParser::get_compilation_database() const
{
    return *compilation_database_ptr;
}

FunctionDeclarationLocation DefinitionGeneratorCmdParser::get_function_declaration_location() const
{
    return declaration_location;
}

void DefinitionGeneratorCmdParser::dump_streams()
{
    std::cerr << err_stream.str();
    std::cout << out_stream.str();
}

} // namespace Tsepepe

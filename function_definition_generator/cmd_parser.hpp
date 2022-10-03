/**
 * @file        cmd_parser.hpp
 * @brief       Parsers the command line arguments for the function definition generator.
 */
#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <sstream>

#include <clang/Tooling/CompilationDatabase.h>

#include "function_declaration_location.hpp"

namespace Tsepepe
{

class DefinitionGeneratorCmdParser
{
  public:
    explicit DefinitionGeneratorCmdParser(int argc, const char** argv);

    const clang::tooling::CompilationDatabase& get_compilation_database() const;
    FunctionDeclarationLocation get_function_declaration_location() const;

    explicit operator bool() const noexcept;
    int get_return_code() const noexcept;

    void dump_streams();

  private:
    struct Error : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    struct ArgumentError : Error
    {
        using Error::Error;
    };

    struct ValueError : Error
    {
        using Error::Error;
    };

    struct Status
    {
        bool can_continue;
        int return_code;
    };

    static void validate_argument_number(int argc, const char** argv);
    static void validate_compilation_database_path(const char* path_to_compile_db);
    static void validate_source_file_exists(const char* path_to_source_file);
    static void validate_line_number_is_numeric(const char* line_number_str);

    static bool is_help_requested(int argc, const char** argv);
    void print_usage(const char* program_path);

    Status validate(int argc, const char** argv);

    std::ostringstream out_stream;
    std::ostringstream err_stream;
    Status status;
    FunctionDeclarationLocation declaration_location;
    std::unique_ptr<clang::tooling::CompilationDatabase> compilation_database_ptr;
};

} // namespace Tsepepe

#endif /* CMD_PARSER_HPP */

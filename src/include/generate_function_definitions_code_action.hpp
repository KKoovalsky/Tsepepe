/**
 * @file        generate_function_definitions_code_action.hpp
 * @brief       Code action which allows to generate function definitions from selected lines.
 */
#ifndef GENERATE_FUNCTION_DEFINITIONS_CODE_ACTION_HPP
#define GENERATE_FUNCTION_DEFINITIONS_CODE_ACTION_HPP

#include <filesystem>

#include <clang/Tooling/CompilationDatabase.h>

#include "directory_tree.hpp"

namespace Tsepepe
{

struct GenerateFunctionDefinitionsCodeActionParameters
{
    std::filesystem::path source_file_path;
    std::string source_file_content;
    unsigned selected_line_begin;
    unsigned selected_line_end;
};

class GenerateFunctionDefinitionsCodeActionLibclangBased
{
  public:
    explicit GenerateFunctionDefinitionsCodeActionLibclangBased(std::shared_ptr<clang::tooling::CompilationDatabase>);

    std::string apply(GenerateFunctionDefinitionsCodeActionParameters);

  private:
    void validate_selected_range(const GenerateFunctionDefinitionsCodeActionParameters&) const;

    std::filesystem::path make_temporary_source_file(const std::filesystem::path& path,
                                                     const std::string& file_content);

    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;

    DirectoryTree tsepepe_temp_directory_tree;
    DirectoryTree this_code_action_directory_tree;
};

} // namespace Tsepepe

#endif /* GENERATE_FUNCTION_DEFINITIONS_CODE_ACTION_HPP */

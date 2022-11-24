/**
 * @file        implement_interface_code_action.hpp
 * @brief       Code Action which implements an interface.
 */
#ifndef IMPLEMENT_INTERFACE_CODE_ACTION_HPP
#define IMPLEMENT_INTERFACE_CODE_ACTION_HPP

#include <filesystem>
#include <string>

#include <clang/Tooling/CompilationDatabase.h>

namespace Tsepepe
{

using NewFileContent = std::string;

struct ImplementInterfaceCodeActionParameters
{
    std::filesystem::path root_directory;
    std::filesystem::path source_file_path;
    std::string source_file_content;
    std::string inteface_name;
    unsigned cursor_position_line;
};

class ImplementIntefaceCodeActionLibclangBased
{
  public:
    explicit ImplementIntefaceCodeActionLibclangBased(std::shared_ptr<clang::tooling::CompilationDatabase>);

    NewFileContent apply(ImplementInterfaceCodeActionParameters);

  private:
    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;
};

}; // namespace Tsepepe

#endif /* IMPLEMENT_INTERFACE_CODE_ACTION_HPP */

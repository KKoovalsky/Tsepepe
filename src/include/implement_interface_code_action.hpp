/**
 * @file        implement_interface_code_action.hpp
 * @brief       Code Action which implements an interface.
 */
#ifndef IMPLEMENT_INTERFACE_CODE_ACTION_HPP
#define IMPLEMENT_INTERFACE_CODE_ACTION_HPP

#include <filesystem>
#include <string>

#include <NamedType/named_type.hpp>

#include <clang/Tooling/CompilationDatabase.h>

#include "common_types.hpp"

namespace Tsepepe
{

using FileContent = fluent::NamedType<std::string, struct FileContentTag>;
using NewFileContent = std::string;
using InterfaceName = fluent::NamedType<std::string, struct InterfaceNameTag>;
using CursorPositionLine = fluent::NamedType<unsigned, struct CursorPositionLineTag>;

struct FileRecord
{
    std::filesystem::path path;
    std::string content;
};

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

    // FIXME: Use ImplementInterfaceCodeActionParameters{.root, .source_file_path, .source_file_content (get rid of
    // FileRecord), .interface_name, .cursor_position_line}
    NewFileContent apply(RootDirectory, const FileRecord&, InterfaceName, CursorPositionLine);

  private:
    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;
};

}; // namespace Tsepepe

#endif /* IMPLEMENT_INTERFACE_CODE_ACTION_HPP */

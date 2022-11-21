/**
 * @file        implement_interface_code_action.hpp
 * @brief       Code Action which implements an interface.
 */
#ifndef IMPLEMENT_INTERFACE_CODE_ACTION_HPP
#define IMPLEMENT_INTERFACE_CODE_ACTION_HPP

#include <filesystem>
#include <string>
#include <vector>

#include <NamedType/named_type.hpp>

#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/CompilationDatabase.h>

#include "common_types.hpp"
#include "directory_tree.hpp"

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

class ImplementIntefaceCodeActionLibclangBased
{
  public:
    explicit ImplementIntefaceCodeActionLibclangBased(std::shared_ptr<clang::tooling::CompilationDatabase>);

    NewFileContent apply(RootDirectory, const FileRecord&, InterfaceName, CursorPositionLine);

  private:
    void build_and_append_ast_unit(const std::filesystem::path&);

    // FIXME: Append to the name: *_and_cache_matched_ast_unit
    const clang::CXXRecordDecl* find_implementor(const FileRecord& file, unsigned cursor_position_line);
    // FIXME: Append to the name: *_and_cache_matched_ast_unit
    const clang::CXXRecordDecl* find_interface(const std::filesystem::path& project_root,
                                               const std::string& iface_name);

    bool is_include_already_in_place(const std::filesystem::path& header_path,
                                     const std::string& cpp_file_content) const;

    Tsepepe::CodeInsertionByOffset
    get_include_statement_code_insertion(const std::string& file_content,
                                         const clang::CXXRecordDecl* interface_declaration) const;

    Tsepepe::CodeInsertionByOffset
    get_overrides_code_insertion(const std::string& file_content,
                                 const clang::CXXRecordDecl* implementor_declaration,
                                 const clang::CXXRecordDecl* interface_declaration) const;

    std::filesystem::path make_temporary_source_file(const FileRecord&);

    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;
    std::vector<std::unique_ptr<clang::ASTUnit>> ast_units;
    DirectoryTree tsepepe_temp_directory_tree;
    DirectoryTree this_code_action_directory_tree;
};

}; // namespace Tsepepe

#endif /* IMPLEMENT_INTERFACE_CODE_ACTION_HPP */

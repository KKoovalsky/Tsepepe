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

namespace Tsepepe
{

using FileContent = fluent::NamedType<std::string, struct FileContentTag>;
using NewFileContent = std::string;
using FileContentConstRef = fluent::NamedType<const std::string&, struct FileContentConstRefTag>;
using InterfaceName = fluent::NamedType<std::string, struct InterfaceNameTag>;
using CursorPositionLine = fluent::NamedType<unsigned, struct CursorPositionLineTag>;

class ImplementIntefaceCodeActionLibclangBased
{
  public:
    explicit ImplementIntefaceCodeActionLibclangBased(std::shared_ptr<clang::tooling::CompilationDatabase>);

    NewFileContent apply(RootDirectory, FileContentConstRef, InterfaceName, CursorPositionLine);

  private:
    void build_and_append_ast_unit(const std::filesystem::path&);
    const clang::CXXRecordDecl* find_interface(const std::string& name, clang::ASTUnit&) const;

    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;
    std::vector<std::unique_ptr<clang::ASTUnit>> ast_units;
};

}; // namespace Tsepepe

#endif /* IMPLEMENT_INTERFACE_CODE_ACTION_HPP */

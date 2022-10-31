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

#include <clang/Tooling/CompilationDatabase.h>

namespace Tsepepe
{

using FileContentAsLines = std::vector<std::string>;
using InterfaceName = fluent::NamedType<std::string, struct InterfaceNameTag>;
using CursorPositionLine = fluent::NamedType<unsigned, struct CursorPositionLineTag>;

class ImplementIntefaceCodeActionLibclangBased
{
  public:
    explicit ImplementIntefaceCodeActionLibclangBased(std::shared_ptr<clang::tooling::CompilationDatabase>);

    FileContentAsLines apply(std::filesystem::path project_root, FileContentAsLines, InterfaceName, CursorPositionLine);

  private:
    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;
};

}; // namespace Tsepepe

#endif /* IMPLEMENT_INTERFACE_CODE_ACTION_HPP */

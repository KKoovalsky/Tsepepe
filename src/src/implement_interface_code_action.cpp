/**
 * @file	implement_interface_code_action.cpp
 * @brief	Implements the Implement Interface code action.
 */

#include "implement_interface_code_action.hpp"

Tsepepe::ImplementIntefaceCodeActionLibclangBased::ImplementIntefaceCodeActionLibclangBased(
    std::shared_ptr<clang::tooling::CompilationDatabase> comp_db) :
    compilation_database(std::move(comp_db))
{
}

Tsepepe::FileContentAsLines Tsepepe::ImplementIntefaceCodeActionLibclangBased::apply(std::filesystem::path project_root,
                                                                                     FileContentAsLines,
                                                                                     InterfaceName,
                                                                                     CursorPositionLine)
{
    return {};
}

/**
 * @file	implement_interface_code_action.cpp
 * @brief	Implements the Implement Interface code action.
 */

#include "implement_interface_code_action.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include "base_error.hpp"
#include "codebase_grepper.hpp"
#include "directory_tree.hpp"
#include "libclang_utils/presumed_source_range.hpp"

using namespace clang;
using namespace clang::tooling;
namespace fs = std::filesystem;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations and definitions
// --------------------------------------------------------------------------------------------------------------------
AST_MATCHER(CXXRecordDecl, isAbstract)
{
    return Node.hasDefinition() and Node.isAbstract();
};

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::ImplementIntefaceCodeActionLibclangBased::ImplementIntefaceCodeActionLibclangBased(
    std::shared_ptr<clang::tooling::CompilationDatabase> comp_db) :
    compilation_database(std::move(comp_db)),
    tsepepe_temp_directory_tree{fs::temp_directory_path() / "tsepepe"},
    this_code_action_directory_tree{fs::temp_directory_path() / "tsepepe" / "impl_iface_code_action"}
{
    ast_units.reserve(4);
}

Tsepepe::NewFileContent
Tsepepe::ImplementIntefaceCodeActionLibclangBased::apply(RootDirectory project_root,
                                                         FileContentConstRef file_content,
                                                         InterfaceName iface_name_alias,
                                                         CursorPositionLine cursor_position_line)
{
    using namespace Tsepepe;

    auto implementor_declaration{find_implementor(file_content.get(), cursor_position_line.get())};
    if (implementor_declaration == nullptr)
        throw BaseError{"No class/struct found under cursor!"};

    const auto& iface_name{iface_name_alias.get()};
    std::string class_definition_regex{"\\b(struct|class)\\s+" + iface_name + "\\b"};
    auto matches{codebase_grep(std::move(project_root), EcmaScriptPattern{class_definition_regex})};

    const CXXRecordDecl* interface_declaration{nullptr};
    for (auto it{matches.begin()}; it != matches.end() and interface_declaration == nullptr; ++it)
    {
        build_and_append_ast_unit(it->path);
        interface_declaration = find_interface(iface_name, *ast_units.back());
    }

    return {};
}

// --------------------------------------------------------------------------------------------------------------------
// Private implementations
// --------------------------------------------------------------------------------------------------------------------
void Tsepepe::ImplementIntefaceCodeActionLibclangBased::build_and_append_ast_unit(const std::filesystem::path& path)
{
    ClangTool tool{*compilation_database, {path.string()}};
    tool.buildASTs(ast_units);
}

const clang::CXXRecordDecl*
Tsepepe::ImplementIntefaceCodeActionLibclangBased::find_interface(const std::string& name, clang::ASTUnit& unit) const
{
    auto abstract_class_matcher{
        ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(name)).bind("abstract class")};
    auto match_result{ast_matchers::match(abstract_class_matcher, unit.getASTContext())};
    if (match_result.size() == 0)
        return nullptr;

    const auto& first_match{match_result[0]};
    return first_match.getNodeAs<CXXRecordDecl>("abstract class");
}

const clang::CXXRecordDecl*
Tsepepe::ImplementIntefaceCodeActionLibclangBased::find_implementor(const std::string& file_content,
                                                                    unsigned cursor_position_line)
{
    auto full_path_to_temp_file{this_code_action_directory_tree.create_file("implementor.hpp", file_content)};

    ClangTool tool{*compilation_database, {full_path_to_temp_file.string()}};
    tool.buildASTs(ast_units);

    auto& ast_unit{*ast_units.back()};
    auto class_matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasDefinition()).bind("class")};
    auto matches{ast_matchers::match(class_matcher, ast_unit.getASTContext())};
    if (matches.empty())
        return nullptr;

    const auto& source_manager{ast_unit.getSourceManager()};
    const CXXRecordDecl* result{nullptr};

    PresumedSourceRange match_source_range;
    for (const auto& match : matches)
    {
        auto node{match.getNodeAs<CXXRecordDecl>("class")};
        if (node == nullptr)
            continue;

        PresumedSourceRange class_source_range{source_manager, node->getBeginLoc(), node->getEndLoc()};
        if (match_source_range.is_surrounding(class_source_range)
            and class_source_range.is_line_inside(cursor_position_line))
        {
            match_source_range = class_source_range;
            result = node;
        }
    }

    return result;
}

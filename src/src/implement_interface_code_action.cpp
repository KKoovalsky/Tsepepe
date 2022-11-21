/**
 * @file	implement_interface_code_action.cpp
 * @brief	Implements the Implement Interface code action.
 */

#include "implement_interface_code_action.hpp"

#include <filesystem>
#include <regex>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include "base_error.hpp"
#include "code_insertions_applier.hpp"
#include "codebase_grepper.hpp"
#include "directory_tree.hpp"
#include "include_statement_place_resolver.hpp"

#include "libclang_utils/base_specifier_resolver.hpp"
#include "libclang_utils/presumed_source_range.hpp"
#include "libclang_utils/pure_virtual_functions_extractor.hpp"
#include "libclang_utils/suitable_place_in_class_finder.hpp"

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

AST_MATCHER_P(CXXRecordDecl, isWithinFile, std::string, filename)
{
    const auto& source_manager{Finder->getASTContext().getSourceManager()};
    return source_manager.getFilename(Node.getLocation()) == filename;
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
                                                         const FileRecord& file_record,
                                                         InterfaceName iface_name,
                                                         CursorPositionLine cursor_position_line)
{
    const auto& file_content{file_record.content};
    auto implementor_declaration{find_implementor(file_record, cursor_position_line.get())};
    if (implementor_declaration == nullptr)
        throw BaseError{"No class/struct found under cursor!"};

    auto interface_declaration{find_interface(project_root.get(), iface_name.get())};
    if (interface_declaration == nullptr)
        throw BaseError{"No interface with the specified name found under the project root directory!"};

    auto include_code_insertion{get_include_statement_code_insertion(file_content, interface_declaration)};
    auto base_class_specifier_insertion{Tsepepe::resolve_base_specifier(
        file_content, implementor_declaration, interface_declaration, ast_units.front()->getSourceManager())};
    auto overrides_insertion{
        get_overrides_code_insertion(file_content, implementor_declaration, interface_declaration)};

    return apply_insertions(file_content,
                            {include_code_insertion, base_class_specifier_insertion, overrides_insertion});
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
Tsepepe::ImplementIntefaceCodeActionLibclangBased::find_interface(const fs::path& project_root,
                                                                  const std::string& iface_name)
{
    std::string class_definition_regex{"\\b(struct|class)\\s+" + iface_name + "\\b"};
    auto file_matches{codebase_grep(RootDirectory(project_root), EcmaScriptPattern{class_definition_regex})};

    auto abstract_class_matcher{
        ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(iface_name)).bind("abstract class")};

    for (const auto& file_match : file_matches)
    {
        build_and_append_ast_unit(file_match.path);
        auto& ast_unit{*ast_units.back()};
        auto match_result{ast_matchers::match(abstract_class_matcher, ast_unit.getASTContext())};
        if (match_result.size() == 0)
            continue;

        const auto& first_match{match_result[0]};
        return first_match.getNodeAs<CXXRecordDecl>("abstract class");
    }

    return nullptr;
}

const clang::CXXRecordDecl*
Tsepepe::ImplementIntefaceCodeActionLibclangBased::find_implementor(const FileRecord& file,
                                                                    unsigned cursor_position_line)
{
    auto full_path_to_temp_file{make_temporary_source_file(file)};

    ClangTool tool{*compilation_database, {full_path_to_temp_file.string()}};
    tool.buildASTs(ast_units);

    auto& ast_unit{*ast_units.back()};
    auto class_matcher{
        ast_matchers::cxxRecordDecl(ast_matchers::hasDefinition(), isWithinFile(full_path_to_temp_file)).bind("class")};
    auto matches{ast_matchers::match(class_matcher, ast_unit.getASTContext())};
    if (matches.empty())
        return nullptr;

    const auto& source_manager{ast_unit.getSourceManager()};
    const CXXRecordDecl* result{nullptr};

    // Get class which has the deepest nesting and has the line under cursor within.
    // FIXME: we can add an AST_MATCHER_P(containsLineNumber, ...) and take the last match, which will be the most
    // deeply nested class that has the line under cursor.
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

bool Tsepepe::ImplementIntefaceCodeActionLibclangBased::is_include_already_in_place(
    const fs::path& header_path, const std::string& cpp_file_content) const
{
    const auto& header_filename{header_path.filename()};
    std::regex re{"#include\\s+\".*?" + header_filename.string() + "\""};
    return std::regex_search(cpp_file_content, re);
}

Tsepepe::CodeInsertionByOffset Tsepepe::ImplementIntefaceCodeActionLibclangBased::get_include_statement_code_insertion(
    const std::string& file_content, const CXXRecordDecl* interface_declaration) const
{
    const auto& interface_file_source_manager{ast_units.back()->getSourceManager()};
    auto interface_header_path{interface_file_source_manager.getFilename(interface_declaration->getLocation()).str()};
    if (is_include_already_in_place(interface_header_path, file_content))
        return {};

    auto include_statement_place{Tsepepe::resolve_include_statement_place(file_content)};

    std::string code{include_statement_place.is_newline_needed ? "\n" : ""};
    code += "#include \"" + fs::path(interface_header_path).filename().string() + "\"\n";
    return {.code = std::move(code), .offset = include_statement_place.offset};
}

Tsepepe::CodeInsertionByOffset Tsepepe::ImplementIntefaceCodeActionLibclangBased::get_overrides_code_insertion(
    const std::string& file_content,
    const CXXRecordDecl* implementor_declaration,
    const CXXRecordDecl* interface_declaration) const
{
    const auto& implementor_file_source_manager{ast_units.front()->getSourceManager()};
    const auto& interface_file_source_manager{ast_units.back()->getSourceManager()};

    auto indentation{
        (Lexer::getIndentationForLine(implementor_declaration->getLocation(), implementor_file_source_manager) + "    ")
            .str()};

    std::string implementor_full_name{implementor_declaration->getQualifiedNameAsString()};
    auto method_overrides{Tsepepe::pure_virtual_functions_to_override_declarations(
        interface_declaration, implementor_full_name, interface_file_source_manager)};
    auto method_overrides_place{Tsepepe::find_suitable_place_in_class_for_public_method(
        file_content, implementor_declaration, implementor_file_source_manager)};

    std::string code{method_overrides_place.is_public_section_needed ? "public:\n" : ""};
    for (auto& override_ : method_overrides)
    {
        code += indentation;
        code += std::move(override_);
        code += '\n';
    }
    return {.code = std::move(code), .offset = method_overrides_place.offset};
}

std::filesystem::path
Tsepepe::ImplementIntefaceCodeActionLibclangBased::make_temporary_source_file(const FileRecord& file)
{
    fs::path temp_file_path;
    if (not fs::is_directory(file.path))
    {
        // Assume is the path is a path to a file.
        std::string fname{".tsepepe_" + file.path.filename().string()};
        temp_file_path = file.path.parent_path() / std::move(fname);
    } else
    {
        temp_file_path = file.path / ".tsepepe_implementor_temp.hpp";
    }

    return this_code_action_directory_tree.create_file(std::move(temp_file_path), file.content);
}

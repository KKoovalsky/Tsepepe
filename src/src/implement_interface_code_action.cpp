/**
 * @file	implement_interface_code_action.cpp
 * @brief	Implements the Implement Interface code action.
 */

#include "implement_interface_code_action.hpp"

#include <filesystem>
#include <memory>
#include <regex>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/Tooling.h>

#include "base_error.hpp"
#include "code_insertions_applier.hpp"
#include "codebase_grepper.hpp"
#include "common_types.hpp"
#include "directory_tree.hpp"
#include "include_statement_place_resolver.hpp"

#include "libclang_utils/ast_record.hpp"
#include "libclang_utils/base_specifier_resolver.hpp"
#include "libclang_utils/presumed_source_range.hpp"
#include "libclang_utils/pure_virtual_functions_extractor.hpp"
#include "libclang_utils/suitable_place_in_class_finder.hpp"

using namespace Tsepepe;
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
// Private helper types
// --------------------------------------------------------------------------------------------------------------------
struct ImplementInterfaceCodeActionParameters
{
    fs::path project_root;
    const FileRecord& source_file;
    std::string iface_name;
    unsigned cursor_position_line;
};

namespace Tsepepe
{

struct ImplementIntefaceCodeActionLibclangBasedImpl
{
    explicit ImplementIntefaceCodeActionLibclangBasedImpl(std::shared_ptr<CompilationDatabase> comp_db,
                                                          ImplementInterfaceCodeActionParameters params) :
        compilation_database{std::move(comp_db)},
        tsepepe_temp_directory_tree{fs::temp_directory_path() / "tsepepe"},
        this_code_action_directory_tree{fs::temp_directory_path() / "tsepepe" / "impl_iface_code_action"},
        parameters{std::move(params)},
        implementor{find_implementor()},
        interface_{find_interface()}
    {
    }

    NewFileContent apply()
    {
        const auto& file_content{parameters.source_file.content};

        auto include_code_insertion{get_include_statement_code_insertion()};
        auto base_class_specifier_insertion{
            Tsepepe::resolve_base_specifier(file_content, implementor, interface_.node)};
        auto overrides_insertion{get_overrides_code_insertion()};

        return apply_insertions(file_content,
                                {include_code_insertion, base_class_specifier_insertion, overrides_insertion});
    }

  private:
    ClangClassRecord find_implementor()
    {
        auto full_path_to_temp_file{make_temporary_source_file(parameters.source_file)};

        ClangTool tool{*compilation_database, {full_path_to_temp_file.string()}};
        tool.buildASTs(ast_units);

        auto& ast_unit{*ast_units.back()};
        auto class_matcher{
            ast_matchers::cxxRecordDecl(ast_matchers::hasDefinition(), isWithinFile(full_path_to_temp_file))
                .bind("class")};
        auto matches{ast_matchers::match(class_matcher, ast_unit.getASTContext())};

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
                and class_source_range.is_line_inside(parameters.cursor_position_line))
            {
                match_source_range = class_source_range;
                result = node;
            }
        }

        if (result == nullptr)
            throw BaseError{"No class/struct found under cursor!"};

        return {.node = result, .source_manager = &source_manager};
    }

    ClangClassRecord find_interface()
    {
        const auto& iface_name{parameters.iface_name};
        std::string class_definition_regex{"\\b(struct|class)\\s+" + iface_name + "\\b"};
        auto file_matches{
            codebase_grep(RootDirectory(parameters.project_root), EcmaScriptPattern{class_definition_regex})};

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
            return {.node = first_match.getNodeAs<CXXRecordDecl>("abstract class"),
                    .source_manager = &ast_unit.getSourceManager()};
        }

        throw BaseError{"No interface with the specified name found under the project root directory!"};
    }

    void build_and_append_ast_unit(const std::filesystem::path& path)
    {
        ClangTool tool{*compilation_database, {path.string()}};
        tool.buildASTs(ast_units);
    }

    CodeInsertionByOffset get_include_statement_code_insertion() const
    {
        auto interface_header_path{interface_.source_manager->getFilename(interface_.node->getLocation()).str()};
        if (is_include_already_in_place(interface_header_path))
            return {};

        auto include_statement_place{Tsepepe::resolve_include_statement_place(parameters.source_file.content)};

        std::string code{include_statement_place.is_newline_needed ? "\n" : ""};
        code += "#include \"" + fs::path(interface_header_path).filename().string() + "\"\n";
        return {.code = std::move(code), .offset = include_statement_place.offset};
    }

    bool is_include_already_in_place(const fs::path& header_path) const
    {
        const auto& header_filename{header_path.filename()};
        std::regex re{"#include\\s+\".*?" + header_filename.string() + "\""};
        return std::regex_search(parameters.source_file.content, re);
    }

    CodeInsertionByOffset get_overrides_code_insertion() const
    {
        auto indentation{
            (Lexer::getIndentationForLine(implementor.node->getLocation(), *implementor.source_manager) + "    ")
                .str()};

        std::string implementor_full_name{implementor.node->getQualifiedNameAsString()};
        auto method_overrides{Tsepepe::pure_virtual_functions_to_override_declarations(
            interface_.node, implementor_full_name, *interface_.source_manager)};
        auto method_overrides_place{Tsepepe::find_suitable_place_in_class_for_public_method(
            parameters.source_file.content, implementor.node, *implementor.source_manager)};

        std::string code{method_overrides_place.is_public_section_needed ? "public:\n" : ""};
        for (auto& override_ : method_overrides)
        {
            code += indentation;
            code += std::move(override_);
            code += '\n';
        }
        return {.code = std::move(code), .offset = method_overrides_place.offset};
    }

    fs::path make_temporary_source_file(const FileRecord& file)
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

    std::shared_ptr<CompilationDatabase> compilation_database;
    std::vector<std::unique_ptr<clang::ASTUnit>> ast_units;

    DirectoryTree tsepepe_temp_directory_tree;
    DirectoryTree this_code_action_directory_tree;

    ImplementInterfaceCodeActionParameters parameters;

    ClangClassRecord implementor;
    ClangClassRecord interface_;
};
} // namespace Tsepepe

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::ImplementIntefaceCodeActionLibclangBased::ImplementIntefaceCodeActionLibclangBased(
    std::shared_ptr<clang::tooling::CompilationDatabase> comp_db) :
    compilation_database(std::move(comp_db))
{
}

Tsepepe::NewFileContent
Tsepepe::ImplementIntefaceCodeActionLibclangBased::apply(RootDirectory project_root,
                                                         const FileRecord& source_file,
                                                         InterfaceName iface_name,
                                                         CursorPositionLine cursor_position_line)
{
    return ImplementIntefaceCodeActionLibclangBasedImpl{compilation_database,
                                                        {.project_root = project_root.get(),
                                                         .source_file = source_file,
                                                         .iface_name = iface_name.get(),
                                                         .cursor_position_line = cursor_position_line.get()}}
        .apply();
}

// --------------------------------------------------------------------------------------------------------------------
// Private implementations
// --------------------------------------------------------------------------------------------------------------------

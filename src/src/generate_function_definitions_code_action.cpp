/**
 * @file	generate_function_definitions_code_action.cpp
 * @brief	Implements the GenerateFunctionDefinitionsCodeActionLibclangBased.
 */
#include "generate_function_definitions_code_action.hpp"

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include <utility>

#include "base_error.hpp"
#include "libclang_utils/full_function_declaration_expander.hpp"
#include "string_utils.hpp"

namespace fs = std::filesystem;
using namespace clang;
using namespace clang::tooling;

AST_MATCHER_P(FunctionDecl, isWithinFile, std::string, filename)
{
    const auto& source_manager{Finder->getASTContext().getSourceManager()};
    return source_manager.getFilename(Node.getLocation()) == filename;
};

using LineRange = std::pair<unsigned, unsigned>;
AST_MATCHER_P(FunctionDecl, isWithinLines, LineRange, line_range)
{
    const auto& source_manager{Finder->getASTContext().getSourceManager()};
    auto actual_line_with_declaration{source_manager.getPresumedLoc(Node.getBeginLoc()).getLine()};
    auto [begin, end] = line_range;
    return actual_line_with_declaration >= begin and actual_line_with_declaration <= end;
}

Tsepepe::GenerateFunctionDefinitionsCodeActionLibclangBased::GenerateFunctionDefinitionsCodeActionLibclangBased(
    std::shared_ptr<clang::tooling::CompilationDatabase> comp_db) :
    compilation_database{std::move(comp_db)},
    tsepepe_temp_directory_tree{fs::temp_directory_path() / "tsepepe"},
    this_code_action_directory_tree{fs::temp_directory_path() / "tsepepe" / "gen_func_def_code_action"}
{
}

std::string Tsepepe::GenerateFunctionDefinitionsCodeActionLibclangBased::apply(
    GenerateFunctionDefinitionsCodeActionParameters params)
{
    validate_selected_range(params);

    auto full_path_to_temp_file{make_temporary_source_file(params.source_file_path, params.source_file_content)};

    std::vector<std::unique_ptr<ASTUnit>> ast_units;
    ClangTool tool{*compilation_database, {full_path_to_temp_file.string()}};
    tool.buildASTs(ast_units);

    auto& ast_unit{*ast_units.back()};
    auto matcher{ast_matchers::functionDecl(ast_matchers::unless(ast_matchers::isDefinition()),
                                            isWithinFile(full_path_to_temp_file),
                                            isWithinLines({params.selected_line_begin, params.selected_line_end}))
                     .bind("function")};
    auto matches{ast_matchers::match(matcher, ast_unit.getASTContext())};
    if (matches.empty())
        return "";

    std::vector<std::string> result_parted;
    auto number_of_potential_declarations(params.selected_line_end - params.selected_line_begin + 1);
    result_parted.reserve(number_of_potential_declarations);

    const auto& source_manager{ast_unit.getSourceManager()};
    for (const auto& match : matches)
    {
        auto node{match.getNodeAs<FunctionDecl>("function")};
        if (node == nullptr)
            continue;
        auto definition{Tsepepe::fully_expand_function_declaration(
            node, source_manager, {.ignore_attribute_specifiers = true, .remove_scope_from_parameters = true})};
        result_parted.emplace_back(std::move(definition));
    }

    auto result{Tsepepe::utils::join(result_parted, "\n{\n}\n\n")};
    result += "\n{\n}\n";
    return result;
}

void Tsepepe::GenerateFunctionDefinitionsCodeActionLibclangBased::validate_selected_range(
    const GenerateFunctionDefinitionsCodeActionParameters& params) const
{
    auto begin{params.selected_line_begin};
    auto end{params.selected_line_end};
    if (begin > end)
        throw Tsepepe::BaseError{"Selected line range must have the end line be past the begin line!"};
}

std::filesystem::path Tsepepe::GenerateFunctionDefinitionsCodeActionLibclangBased::make_temporary_source_file(
    const std::filesystem::path& path, const std::string& file_content)
{
    fs::path temp_file_path;
    if (not fs::is_directory(path))
    {
        // Assume is the path is a path to a file.
        std::string fname{".tsepepe_" + path.filename().string()};
        temp_file_path = path.parent_path() / std::move(fname);
    } else
    {
        temp_file_path = path / ".tsepepe_func_decls_temp.hpp";
    }

    return this_code_action_directory_tree.create_file(std::move(temp_file_path), file_content);
}


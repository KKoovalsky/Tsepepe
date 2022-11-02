/**
 * @file	implement_interface_code_action.cpp
 * @brief	Implements the Implement Interface code action.
 */

#include "implement_interface_code_action.hpp"

#include <fstream>
#include <memory>
#include <sstream>

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include "codebase_grepper.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"

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

static std::string load_file(const fs::path& path);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::ImplementIntefaceCodeActionLibclangBased::ImplementIntefaceCodeActionLibclangBased(
    std::shared_ptr<clang::tooling::CompilationDatabase> comp_db) :
    compilation_database(std::move(comp_db))
{
    ast_units.reserve(4);
}

Tsepepe::NewFileContent Tsepepe::ImplementIntefaceCodeActionLibclangBased::apply(RootDirectory project_root,
                                                                                 FileContentConstRef,
                                                                                 InterfaceName iface_name_alias,
                                                                                 CursorPositionLine)
{
    using namespace Tsepepe;

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

static std::string load_file(const fs::path& path)
{
    std::ifstream ifs{path};
    std::stringstream sstr;
    while (ifs >> sstr.rdbuf())
        ;
    return sstr.str();
}


/**
 * @file        clang_ast_fixtures.hpp
 * @brief       Clang AST test fixtures.
 */
#ifndef CLANG_AST_FIXTURES_HPP
#define CLANG_AST_FIXTURES_HPP

#include <filesystem>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

namespace Tsepepe
{

struct ClangSingleAstFixture
{
    explicit ClangSingleAstFixture(const std::string& header_file_content) :
        header_file_content{header_file_content},
        ast_unit{clang::tooling::buildASTFromCodeWithArgs(header_file_content, {"-std=gnu++20"})}
    {
    }

    template<typename NodeType, typename Matcher>
    auto get_first_match(const Matcher& matcher) const
    {
        std::string matcher_id{matcher.getID().first.asStringRef()};

        auto matches{clang::ast_matchers::match(matcher, ast_unit->getASTContext())};
        if (matches.size() == 0)
            throw std::runtime_error{"Matcher: " + matcher_id + ", failed to find a match within the file content:\n"
                                     + header_file_content};

        const auto& first_match{matches[0]};
        // Normally this works, but in this case, getNodeAs returns nullptr, for the CXXMethodDecl, thus inside
        // there is a workaround, with getMap() ...
        auto node{first_match.template getNodeAs<NodeType>(matcher_id)};
        if (node == nullptr)
        {
            node = first_match.getMap().begin()->second.template get<NodeType>();
            if (node == nullptr)
                throw std::runtime_error{"Failed to get node for matcher: " + matcher_id
                                         + ", within the file content:\n" + header_file_content};
        }
        return node;
    }

    const clang::SourceManager& get_source_manager() const
    {
        return ast_unit->getSourceManager();
    }

    const clang::ASTUnit& get_ast_unit() const
    {
        return *ast_unit;
    }

  private:
    std::string header_file_content;
    std::unique_ptr<clang::ASTUnit> ast_unit;
};

struct ClangAstFixture
{
    using CppFiles = std::vector<std::filesystem::path>;
    using ASTUnits = std::vector<std::unique_ptr<clang::ASTUnit>>;
    using CompilationDatabase = std::unique_ptr<clang::tooling::CompilationDatabase>;

    ClangAstFixture(std::filesystem::path path_to_compile_db, CppFiles cpp_files_) :
        compilation_database{clang::tooling::CompilationDatabase::loadFromDirectory(path_to_compile_db.string(),
                                                                                    compilation_db_build_error_str)},
        cpp_files{std::move(cpp_files_)}
    {
        if (compilation_database == nullptr)
            throw std::runtime_error{"Failed to build the compilation database: " + compilation_db_build_error_str};

        if (cpp_files.empty())
            throw std::runtime_error{"The C++ file list cannot be empty!"};

        namespace fs = std::filesystem;
        std::vector<std::string> cpp_files_raw;
        cpp_files_raw.reserve(cpp_files.size());
        std::ranges::transform(
            cpp_files, std::back_inserter(cpp_files_raw), [](const fs::path& p) { return p.string(); });

        clang::tooling::ClangTool clang_tool{*(compilation_database), cpp_files_raw};
        clang_tool.buildASTs(asts);
    }

    template<typename NodeType, typename Matcher>
    auto get_first_match(const Matcher& matcher) const
    {
        std::string matcher_id{matcher.getID().first.asStringRef()};

        for (const auto& ast_unit : asts)
        {
            auto matches{clang::ast_matchers::match(matcher, ast_unit->getASTContext())};
            if (matches.size() == 0)
                continue;

            const auto& first_match{matches[0]};
            auto node{first_match.template getNodeAs<NodeType>(matcher_id)};
            if (node != nullptr)
                return node;

            node = first_match.getMap().begin()->second.template get<NodeType>();
            if (node != nullptr)
                return node;
        }

        throw std::runtime_error{"Failed to get node for matcher: " + matcher_id};
    }

    const clang::SourceManager& get_source_manager() const
    {
        return asts[0]->getSourceManager();
    }

  private:
    std::string compilation_db_build_error_str;
    CompilationDatabase compilation_database;
    CppFiles cpp_files;
    ASTUnits asts;
};

} // namespace Tsepepe

#endif /* CLANG_AST_FIXTURES_HPP */

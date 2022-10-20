/**
 * @file        clang_ast_utils.hpp
 * @brief       Clang AST utilities.
 */
#ifndef CLANG_AST_UTILS_HPP
#define CLANG_AST_UTILS_HPP

#include <filesystem>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CompilationDatabase.h>

namespace Tsepepe::utils::clang_ast
{

//! Throws if no compilation database is found under the specified directory, or when the directory doesn't exist.
std::unique_ptr<clang::tooling::CompilationDatabase>
parse_compilation_database(const std::filesystem::path& directory_with_compilation_database);

struct MatchCounter : clang::ast_matchers::MatchFinder::MatchCallback
{
  public:
    void run(const clang::ast_matchers::MatchFinder::MatchResult&) override
    {
        ++match_count;
    }

    unsigned get_count() const
    {
        return match_count;
    };

  private:
    unsigned match_count{0};
};

struct MatchValidator : MatchCounter
{
    bool is_match_found() const
    {
        return get_count() > 0;
    };
};

} // namespace Tsepepe::utils::clang_ast

#endif /* CLANG_AST_UTILS_HPP */

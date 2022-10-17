/**
 * @file        clang_ast_utils.hpp
 * @brief       Clang AST utilities.
 */
#ifndef CLANG_AST_UTILS_HPP
#define CLANG_AST_UTILS_HPP

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

namespace Tsepepe::utils
{

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

} // namespace Tsepepe::utils

#endif /* CLANG_AST_UTILS_HPP */

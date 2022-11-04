/**
 * @file        clang_ast_fixtures.hpp
 * @brief       Clang AST test fixtures.
 */
#ifndef CLANG_AST_FIXTURES_HPP
#define CLANG_AST_FIXTURES_HPP

#include <string>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include <iostream>

namespace Tsepepe
{

struct ClangSingleAstFixture
{
    explicit ClangSingleAstFixture(const std::string& header_file_content) :
        header_file_content{header_file_content}, ast_unit{clang::tooling::buildASTFromCode(header_file_content)}
    {
    }

    template<typename NodeType, typename Matcher>
    auto get_first_match(const Matcher& matcher)
    {
        std::string matcher_id{matcher.getID().first.asStringRef()};

        auto matches{clang::ast_matchers::match(matcher, ast_unit->getASTContext())};
        if (matches.size() == 0)
            throw std::runtime_error{"Matcher: " + matcher_id + ", failed to find a match within the file content:\n"
                                     + header_file_content};

        std::cout << matches.size() << std::endl;
        const auto& first_match{matches[0]};
        auto node{first_match.template getNodeAs<NodeType>(matcher_id)};
        if (node == nullptr)
            throw std::runtime_error{"Failed to get node for matcher: " + matcher_id + ", within the file content:\n"
                                     + header_file_content};
        return node;
    }

  private:
    std::string header_file_content;
    std::unique_ptr<clang::ASTUnit> ast_unit;
};

} // namespace Tsepepe

#endif /* CLANG_AST_FIXTURES_HPP */

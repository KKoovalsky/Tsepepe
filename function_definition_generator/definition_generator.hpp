/**
 * @file        definition_generator.hpp
 * @brief       Declares a generator which is a matcher for the Clang AST traversal.
 */
#ifndef DEFINITION_GENERATOR_HPP
#define DEFINITION_GENERATOR_HPP

#include <string>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

namespace CppTinyRefactor
{
struct FileWithDeclaration
{
    explicit FileWithDeclaration(std::string v) : value{std::move(v)}
    {
    }

    std::string value;
};

struct LineWithDeclaration
{
    explicit LineWithDeclaration(unsigned v) : value{v}
    {
    }

    unsigned value;
};

class DefinitionGenerator final : public clang::ast_matchers::MatchFinder::MatchCallback
{
  public:
    explicit DefinitionGenerator(FileWithDeclaration, LineWithDeclaration);

    void run(const clang::ast_matchers::MatchFinder::MatchResult&) override;

  private:
    void print_return_type_if_any(const clang::CXXMethodDecl*) const;
    void print_name(const clang::CXXMethodDecl*) const;
    void print_parameters(const clang::CXXMethodDecl*) const;
    void print_const_qualifier_if_has_one(const clang::CXXMethodDecl*) const;

    std::string file;
    unsigned line_number;

    clang::PrintingPolicy printing_policy;
};

}; // namespace CppTinyRefactor

#endif /* DEFINITION_GENERATOR_HPP */

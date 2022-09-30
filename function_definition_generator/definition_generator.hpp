/**
 * @file        definition_generator.hpp
 * @brief       Declares a generator which is a matcher for the Clang AST traversal.
 */
#ifndef DEFINITION_GENERATOR_HPP
#define DEFINITION_GENERATOR_HPP

#include <sstream>
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

namespace detail
{

class DefinitionPrinter final : public clang::ast_matchers::MatchFinder::MatchCallback
{
  public:
    explicit DefinitionPrinter(FileWithDeclaration, LineWithDeclaration);

    void run(const clang::ast_matchers::MatchFinder::MatchResult&) override;

    //! Returns the definition for the corresponding declaration.
    std::string get() const;

  private:
    using Decl = clang::FunctionDecl;

    void print_return_type_if_any(const Decl*);
    void print_name(const Decl*);
    void print_parameters(const Decl*);
    void print_const_qualifier_if_has_one(const Decl*);

    std::string file;
    unsigned line_number;

    clang::PrintingPolicy printing_policy;

    std::ostringstream output_stream;
};

} // namespace detail

class DefinitionGenerator : public clang::ast_matchers::MatchFinder
{
  public:
    explicit DefinitionGenerator(FileWithDeclaration, LineWithDeclaration);

    //! Returns the definition for the corresponding declaration.
    std::string get() const;

  private:
    detail::DefinitionPrinter printer;
    clang::ast_matchers::DeclarationMatcher matcher;
};
}; // namespace CppTinyRefactor

#endif /* DEFINITION_GENERATOR_HPP */

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

#include "function_declaration_location.hpp"

namespace Tsepepe
{

namespace detail
{

class DefinitionPrinter final : public clang::ast_matchers::MatchFinder::MatchCallback
{
  public:
    explicit DefinitionPrinter(FunctionDeclarationLocation);

    void run(const clang::ast_matchers::MatchFinder::MatchResult&) override;

    //! Returns the definition for the corresponding declaration.
    std::string get() const;

  private:
    using Decl = clang::FunctionDecl;

    void print_return_type_if_any(const Decl*, const clang::SourceManager&);
    void print_name(const Decl*);
    void print_parameters(const Decl*, const clang::SourceManager&);
    void print_const_qualifier_if_has_one(const Decl*);
    void print_ref_qualifier_if_has_one(const Decl*);
    void print_noexcept_qualifier_if_has_one(const Decl* node, const clang::SourceManager&);

    std::string source_range_content_to_string(const clang::SourceRange&, const clang::SourceManager&) const;
    bool has_explicit_return_type(const Decl*, const clang::SourceManager&) const;

    FunctionDeclarationLocation declaration_location;

    clang::LangOptions lang_options;
    clang::PrintingPolicy printing_policy;

    std::ostringstream output_stream;
};

} // namespace detail

class DefinitionGenerator : public clang::ast_matchers::MatchFinder
{
  public:
    explicit DefinitionGenerator(FunctionDeclarationLocation);

    //! Returns the definition for the corresponding declaration.
    std::string get() const;

  private:
    detail::DefinitionPrinter printer;
    clang::ast_matchers::DeclarationMatcher matcher;
};
}; // namespace Tsepepe

#endif /* DEFINITION_GENERATOR_HPP */

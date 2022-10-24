/**
 * @file	finder.cpp
 * @brief	Implements the core of the suitable place in class finder.
 */
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>

#include <algorithm>
#include <string>

#include <iostream>

#include "finder.hpp"

#include "clang_ast_utils.hpp"

using namespace clang;
using namespace clang::tooling;
namespace ast = clang::ast_matchers;

// --------------------------------------------------------------------------------------------------------------------
// Private stuff
// --------------------------------------------------------------------------------------------------------------------
AST_MATCHER_P(CXXRecordDecl, isMemberOf, std::string, name)
{
    return false;
};

class LineFinder : public ast::MatchFinder::MatchCallback
{
  public:
    LineFinder() : printing_policy{lang_options}
    {
        printing_policy.adjustForCPlusPlus();
    }

    void run(const ast::MatchFinder::MatchResult& result) override
    {
        auto node{result.Nodes.getNodeAs<CXXRecordDecl>("class")};
        if (node == nullptr)
            return;

        const auto& source_manager{result.Context->getSourceManager()};
        auto last_public_method_in_first_public_method_chain{find_last_public_method_in_first_public_chain(node)};

        if (last_public_method_in_first_public_method_chain != nullptr)
        {
            auto past_end_source_loc{last_public_method_in_first_public_method_chain->getEndLoc()};
            auto end_source_loc{Lexer::getLocForEndOfToken(past_end_source_loc, 9, source_manager, lang_options)};
            auto r{source_manager.getSpellingLineNumber(end_source_loc)};
            found_line_number = r;
        }
    }

    std::optional<unsigned> get_result() const
    {
        return found_line_number;
    }

  private:
    const CXXMethodDecl* find_last_public_method_in_first_public_chain(const CXXRecordDecl* record) const
    {
        auto is_public_method{[](const CXXMethodDecl* method) {
            return method->getAccess() == AccessSpecifier::AS_public;
        }};

        auto is_not_public_method{[&](const CXXMethodDecl* method) {
            return not is_public_method(method);
        }};

        auto beg{record->method_begin()};
        auto end{record->method_end()};

        auto first_public_method_it{std::ranges::find_if(beg, end, is_public_method)};
        if (first_public_method_it == end)
            return nullptr;

        first_public_method_it->dump();

        // Ideally, we could use find_if to find the first non-public method, and then use std::prev(), but the method
        // range is a forward range.
        auto last_public_method_in_first_public_method_chain_it{first_public_method_it};
        for (auto it{first_public_method_it}; it != end; ++it)
        {
            if (is_not_public_method(*it) or it->isImplicit())
                break;
            last_public_method_in_first_public_method_chain_it = it;
        }

        last_public_method_in_first_public_method_chain_it->dump();
        return *last_public_method_in_first_public_method_chain_it;
    }

    clang::LangOptions lang_options;
    clang::PrintingPolicy printing_policy;

    std::optional<unsigned> found_line_number;
};

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::optional<Tsepepe::SuitablePlaceInClassFinder::Line> Tsepepe::SuitablePlaceInClassFinder::find(const Input& input)
{
    auto class_matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasName(input.class_name)).bind("class")};

    ast::MatchFinder finder;
    LineFinder line_finder;
    finder.addMatcher(class_matcher, &line_finder);

    ClangTool tool{*input.compilation_database_ptr, {input.header_file}};

    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());

    return line_finder.get_result();
}

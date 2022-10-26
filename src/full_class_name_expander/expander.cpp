/**
 * @file	extractor.cpp
 * @brief	Implements the pure virtual functions extractor.
 */

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include <regex>

#include "expander.hpp"

#include "clang_ast_utils.hpp"

using namespace clang;
using namespace clang::tooling;
namespace ast = clang::ast_matchers;

class ClassNameExpander : public ast::MatchFinder::MatchCallback
{
  public:
    void run(const ast::MatchFinder::MatchResult& result) override
    {
        auto node{result.Nodes.getNodeAs<CXXRecordDecl>("class")};
        if (node == nullptr)
            return;

        full_class_name = node->getQualifiedNameAsString();
    }

    const std::string& get_result()
    {
        return full_class_name;
    }

  private:
    std::string full_class_name;
};

std::string Tsepepe::FullClassNameExpander::expand(const Input& input)
{
    auto class_matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasName(input.class_name),
                                                   ast_matchers::unless(ast_matchers::isImplicit()))
                           .bind("class")};

    ast::MatchFinder finder;
    ClassNameExpander expander;
    finder.addMatcher(class_matcher, &expander);

    ClangTool tool{*input.compilation_database_ptr, {input.header_file}};

    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());

    return expander.get_result();
}

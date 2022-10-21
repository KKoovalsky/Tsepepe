/**
 * @file	extractor.cpp
 * @brief	Implements the pure virtual functions extractor.
 */

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include <regex>

#include "extractor.hpp"

#include "clang_ast_utils.hpp"

using namespace clang;
using namespace clang::tooling;
namespace ast = clang::ast_matchers;

AST_MATCHER_P(CXXMethodDecl, isMemberOf, std::string, class_name)
{
    return Node.getParent()->getNameAsString() == class_name;
};

class MatchCollector : public ast::MatchFinder::MatchCallback
{
  public:
    MatchCollector() : printing_policy{lang_options}
    {
        printing_policy.adjustForCPlusPlus();
    }

    void run(const ast::MatchFinder::MatchResult& result) override
    {
        auto node{result.Nodes.getNodeAs<CXXMethodDecl>("pure")};
        auto declaration{Tsepepe::utils::clang_ast::source_range_content_to_string(
            node->getSourceRange(), result.Context->getSourceManager(), lang_options)};

        std::regex overrider{"virtual\\s+(.*)(\\s+=\\s+0)"};
        auto override_declaration{std::regex_replace(declaration, overrider, "$1 override;")};

        function_override_declarations.emplace_back(std::move(override_declaration));
    }

    const std::vector<std::string>& get_result()
    {
        return function_override_declarations;
    }

  private:
    std::vector<std::string> function_override_declarations;
    clang::LangOptions lang_options;
    clang::PrintingPolicy printing_policy;
};

std::vector<std::string> Tsepepe::PureVirtualFunctionsExtractor::extract(const Input& input)
{
    auto pure_virtual_functions_for_specific_class_matcher{
        ast::cxxMethodDecl(ast::isPure(), isMemberOf(input.class_name)).bind("pure")};

    ast::MatchFinder finder;
    MatchCollector collector;
    finder.addMatcher(pure_virtual_functions_for_specific_class_matcher, &collector);

    ClangTool tool{*input.compilation_database_ptr, {input.header_file}};

    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());

    return collector.get_result();
}

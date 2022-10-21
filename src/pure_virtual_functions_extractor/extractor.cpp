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

AST_MATCHER(CXXRecordDecl, isAbstract)
{
    return Node.hasDefinition() and Node.isAbstract();
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
        auto node{result.Nodes.getNodeAs<CXXRecordDecl>("abstract class")};
        if (node == nullptr)
            return;
        const auto& source_manager{result.Context->getSourceManager()};
        node->forallBases([&](const CXXRecordDecl* base) {
            collect_override_declarations(base, source_manager);
            return true;
        });
        collect_override_declarations(node, source_manager);
    }

    const std::vector<std::string>& get_result()
    {
        return function_override_declarations;
    }

  private:
    void collect_override_declarations(const CXXRecordDecl* record, const SourceManager& source_manager)
    {
        for (auto method : record->methods())
            if (method->isPure())
                append_override_declaration(method, source_manager);
    }

    void append_override_declaration(const CXXMethodDecl* method, const SourceManager& source_manager)
    {
        auto declaration{Tsepepe::utils::clang_ast::source_range_content_to_string(
            method->getSourceRange(), source_manager, lang_options)};

        std::regex overrider{"virtual\\s+(.*)(\\s+=\\s+0)"};
        auto override_declaration{std::regex_replace(declaration, overrider, "$1 override;")};

        function_override_declarations.emplace_back(std::move(override_declaration));
    }

    std::vector<std::string> function_override_declarations;
    clang::LangOptions lang_options;
    clang::PrintingPolicy printing_policy;
};

std::vector<std::string> Tsepepe::PureVirtualFunctionsExtractor::extract(const Input& input)
{
    auto abstract_class_matcher{
        ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(input.class_name)).bind("abstract class")};

    ast::MatchFinder finder;
    MatchCollector collector;
    finder.addMatcher(abstract_class_matcher, &collector);

    ClangTool tool{*input.compilation_database_ptr, {input.header_file}};

    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());

    return collector.get_result();
}

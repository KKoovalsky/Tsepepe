/**
 * @file	tool.cpp
 * @brief	The main app entry for the function definition generator.
 */
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include "definition_generator.hpp"

using namespace clang;
using namespace llvm::cl;
using namespace clang::tooling;
using namespace clang::ast_matchers;

using namespace CppTinyRefactor;

int main(int argc, const char** argv)
{
    OptionCategory cat("my-tool options");
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, cat);
    if (!ExpectedParser)
    {
        // Fail gracefully for unsupported options.
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    IgnoringDiagConsumer diagnostic_consumer;
    CommonOptionsParser& OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    Tool.setDiagnosticConsumer(&diagnostic_consumer);

    DefinitionGenerator generator{FileWithDeclaration{OptionsParser.getSourcePathList()[0]}, LineWithDeclaration{44}};
    MatchFinder finder;
    DeclarationMatcher matcher{cxxMethodDecl().bind("method declaration")};
    finder.addMatcher(matcher, &generator);

    return Tool.run(newFrontendActionFactory(&finder).get());
}

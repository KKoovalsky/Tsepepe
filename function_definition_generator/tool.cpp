/**
 * @file	tool.cpp
 * @brief	The main app entry for the function definition generator.
 */
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include "definition_generator.hpp"

using namespace clang;
using namespace llvm::cl;
using namespace clang::tooling;

using namespace CppTinyRefactor;

int main(int argc, const char** argv)
{
    OptionCategory cat("Function Definition Generator options");
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, cat, NumOccurrencesFlag::OneOrMore, "MAKAPAKA");
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

    DefinitionGenerator generator{FileWithDeclaration{OptionsParser.getSourcePathList()[0]}, LineWithDeclaration{3}};
    return Tool.run(newFrontendActionFactory(&generator).get());
}

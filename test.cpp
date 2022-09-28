#include <iostream>

// Declares clang::SyntaxOnlyAction.
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
// Declares llvm::cl::extrahelp.
#include <llvm/Support/CommandLine.h>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

using namespace clang::tooling;
using namespace llvm;

using namespace clang;
using namespace clang::ast_matchers;

class MethodDeclarationPrinter : public MatchFinder::MatchCallback
{
  public:
    void run(const MatchFinder::MatchResult& result) override
    {
        if (auto node{result.Nodes.getNodeAs<clang::CXXMethodDecl>("method declaration")}; node != nullptr)
        {
            auto& source_manager{result.Context->getSourceManager()};
            auto source_location{node->getLocation()};
            auto line_nr{source_manager.getPresumedLoc(source_location).getLine()};
            std::cout << "Line nr: " << line_nr << std::endl;
            node->dump();
        }
    }
};

namespace CppTinyRefactor
{
struct MethodDeclarationFile
{
    explicit MethodDeclarationFile(std::string v) : value{std::move(v)}
    {
    }

    std::string value;
};

struct MethodDeclarationLineNumber
{
    explicit MethodDeclarationLineNumber(unsigned v) : value{v}
    {
    }
    unsigned value;
};
}; // namespace CppTinyRefactor

using namespace CppTinyRefactor;

class MethodDefinitionGenerator final : public MatchFinder::MatchCallback
{
  public:
    MethodDefinitionGenerator(MethodDeclarationFile f, MethodDeclarationLineNumber ln) :
        line_number{ln.value}, file{std::move(f.value)}
    {
    }

    void run(const MatchFinder::MatchResult& result) override
    {
        auto node{result.Nodes.getNodeAs<clang::CXXMethodDecl>("method declaration")};
        if (node == nullptr)
            return;

        auto& source_manager{result.Context->getSourceManager()};
        auto source_location{node->getLocation()};
        auto presumed_loc{source_manager.getPresumedLoc(source_location)};

        auto matched_node_file_name{presumed_loc.getFilename()};
        auto matched_node_line_nr{presumed_loc.getLine()};

        if (matched_node_line_nr != line_number or matched_node_file_name != file)
            return;

        if (node->isImplicit())
            return;

        auto return_type_str{node->getReturnType().getAsString()};
        auto name_str{node->getQualifiedNameAsString()};

        std::cout << return_type_str << ' ' << name_str << "()";
        if (node->isConst())
            std::cout << " const";
        std::cout << std::endl;
    }

  private:
    std::string file;
    unsigned line_number;
};

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
// static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
// static cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char** argv)
{
    llvm::cl::OptionCategory cat("my-tool options");
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

    MethodDefinitionGenerator generator{MethodDeclarationFile{OptionsParser.getSourcePathList()[0]},
                                        MethodDeclarationLineNumber{34}};
    MatchFinder finder;
    DeclarationMatcher matcher{cxxMethodDecl().bind("method declaration")};
    finder.addMatcher(matcher, &generator);

    return Tool.run(newFrontendActionFactory(&finder).get());
}

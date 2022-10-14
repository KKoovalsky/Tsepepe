#include <iostream>
#include <string>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/Support/raw_os_ostream.h>

using namespace clang;
using namespace clang::tooling;

AST_MATCHER(CXXRecordDecl, isAbstract)
{
    return Node.hasDefinition() and Node.isAbstract();
};

struct Validator : ast_matchers::MatchFinder::MatchCallback
{
  public:
    void run(const ast_matchers::MatchFinder::MatchResult&) override
    {
        match_found = true;
    }

    bool is_match_found() const
    {
        return match_found;
    }

  private:
    bool match_found{false};
};

int main(int argc, const char* argv[])
{
    std::string compilation_database_dir{argv[1]};
    std::string header_file{argv[2]};
    std::string class_name{argv[3]};

    std::string err;
    auto compilation_database_ptr{
        clang::tooling::CompilationDatabase::loadFromDirectory(compilation_database_dir, err)};

    auto abstract_class_matcher{
        ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(class_name)).bind("abstract class")};

    ClangTool tool{*compilation_database_ptr, {header_file}};

    ast_matchers::MatchFinder finder;
    Validator validator;
    finder.addMatcher(abstract_class_matcher, &validator);

    auto return_code{tool.run(newFrontendActionFactory(&finder).get())};
    std::cout << "Match found?: " << validator.is_match_found() << std::endl;
    return return_code;
}

/**
 * @file	finder.cpp
 * @brief	Defines the Abstract Class Finder.
 */
#include <ranges>
#include <string>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include <boost/process.hpp>

#include <range/v3/range/conversion.hpp>

#include "finder.hpp"

using namespace clang;
using namespace clang::tooling;

namespace fs = std::filesystem;
namespace views = std::ranges::views;

// --------------------------------------------------------------------------------------------------------------------
// Helper AST definitions
// --------------------------------------------------------------------------------------------------------------------
AST_MATCHER(CXXRecordDecl, isAbstract)
{
    return Node.hasDefinition() and Node.isAbstract();
};

namespace Tsepepe::AbstractClassFinder
{

// --------------------------------------------------------------------------------------------------------------------
// Helpers declaration
// --------------------------------------------------------------------------------------------------------------------
static std::vector<fs::path> ripgrep_for_class_name(const fs::path& root, const std::string& class_name);
static bool has_abstract_class(const fs::path& header, const CompilationDatabase&, const std::string& class_name);

// --------------------------------------------------------------------------------------------------------------------
// Helper templates
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------

std::vector<fs::path> find(const Input& input)
{
    auto files_having_searched_class{ripgrep_for_class_name(input.project_root, input.class_name)};
    auto files_having_abstract_class{files_having_searched_class | views::filter([&](const fs::path& header) {
                                         return has_abstract_class(
                                             header, *input.compilation_database_ptr, input.class_name);
                                     })
                                     | ranges::to_vector};
    return files_having_abstract_class;
}

// --------------------------------------------------------------------------------------------------------------------
// Helpers definition
// --------------------------------------------------------------------------------------------------------------------
static std::vector<fs::path> ripgrep_for_class_name(const fs::path& root, const std::string& class_name)
{
    std::string class_definition_regex{"(^|\\s+)(struct|class)\\s+" + class_name + "\\b"};
    std::string command{"rg " + class_definition_regex + " " + root.string() + " -l"};

    std::vector<fs::path> result;

    using namespace boost::process;
    ipstream pipe_stream;
    child c{std::move(command), std_out > pipe_stream};
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        result.emplace_back(std::move(line));
    c.wait();

    return result;
}

static bool
has_abstract_class(const fs::path& header, const CompilationDatabase& comp_db, const std::string& class_name)
{
    auto abstract_class_matcher{
        ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(class_name)).bind("abstract class")};

    ast_matchers::MatchFinder finder;
    Tsepepe::utils::clang_ast::MatchValidator validator;
    finder.addMatcher(abstract_class_matcher, &validator);

    ClangTool tool{comp_db, {header}};
    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());
    return validator.is_match_found();
}

} // namespace Tsepepe::AbstractClassFinder


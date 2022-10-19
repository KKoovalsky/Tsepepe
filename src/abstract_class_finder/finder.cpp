/**
 * @file	finder.cpp
 * @brief	Defines the Abstract Class Finder.
 */

#include <algorithm>
#include <ranges>
#include <regex>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include <iostream>

#include "clang_ast_utils.hpp"
#include "finder.hpp"
#include "utils.hpp"

using namespace clang;
using namespace clang::tooling;

namespace fs = std::filesystem;

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
static bool try_find_in_matching_files(const Input& input);
static bool try_find_in_any_header(const Input& input);
static bool has_abstract_class(const fs::path& header, const CompilationDatabase&, const std::string& class_name);

// --------------------------------------------------------------------------------------------------------------------
// Helper templates
// --------------------------------------------------------------------------------------------------------------------
template<typename Files>
bool find_abstract_class(Files&& files, const Input& input)
{
    for (auto file : files)
    {
        if (has_abstract_class(file, *input.compilation_database_ptr, input.class_name))
        {
            std::cout << file.path().string();
            return true;
        }
    }

    return false;
}

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------

ReturnCode find(const Input& input)
{
    if (not try_find_in_matching_files(input))
        if (not try_find_in_any_header(input))
            return 1;
    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Helpers definition
// --------------------------------------------------------------------------------------------------------------------
static bool try_find_in_matching_files(const Input& input)
{
    auto matching_name_pattern{Tsepepe::utils::class_name_to_header_file_regex(input.class_name)};
    std::regex matching_name_regex{matching_name_pattern, std::regex::ECMAScript | std::regex::icase};
    auto make_file_name_matcher_by_regex{[](const std::regex& re) {
        return [re](const fs::directory_entry& dir_entry) {
            auto fname{dir_entry.path().filename().string()};
            return std::regex_match(fname, re);
        };
    }};

    auto is_file_name_matching_regex{make_file_name_matcher_by_regex(matching_name_regex)};
    auto header_files_matched_by_class_name{fs::recursive_directory_iterator{input.project_root}
                                            | std::views::filter(is_file_name_matching_regex)};

    return find_abstract_class(header_files_matched_by_class_name, input);
}

static bool try_find_in_any_header(const Input& input)
{
    auto is_header_file{[](const fs::directory_entry& dir_entry) {
        auto ext{dir_entry.path().extension().string()};
        return ext == ".h" or ext == ".hpp" or ext == ".hh" or ext == ".hxx";
    }};
    auto headers{fs::recursive_directory_iterator{input.project_root} | std::views::filter(is_header_file)};
    return find_abstract_class(headers, input);
}

static bool
has_abstract_class(const fs::path& header, const CompilationDatabase& comp_db, const std::string& class_name)
{
    auto abstract_class_matcher{
        ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(class_name)).bind("abstract class")};

    ast_matchers::MatchFinder finder;
    Tsepepe::utils::MatchValidator validator;
    finder.addMatcher(abstract_class_matcher, &validator);

    ClangTool tool{comp_db, {header}};
    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());
    return validator.is_match_found();
}

} // namespace Tsepepe::AbstractClassFinder


/**
 * @file	finder.cpp
 * @brief	Defines the Abstract Class Finder.
 */

#include <algorithm>
#include <experimental/array>
#include <ranges>
#include <regex>
#include <string_view>

#include <iostream>

#include "finder.hpp"
#include "utils.hpp"

using namespace clang;
using namespace clang::tooling;

namespace fs = std::filesystem;

AST_MATCHER(CXXRecordDecl, isAbstract)
{
    return Node.hasDefinition() and Node.isAbstract();
};

Tsepepe::AbstractClassFinder::Finder::Finder(Input input)
{
    // auto abstract_class_matcher{
    //     ast_matchers::cxxRecordDecl(isAbstract(), ast_matchers::hasName(class_name)).bind("abstract class")};
}

namespace Tsepepe::AbstractClassFinder
{

// --------------------------------------------------------------------------------------------------------------------
// Helpers declaration
// --------------------------------------------------------------------------------------------------------------------
static void try_find_in_matching_files(const Input& input);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------

ReturnCode find(const Input& input)
{
    try_find_in_matching_files(input);
    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Helpers definition
// --------------------------------------------------------------------------------------------------------------------
static void try_find_in_matching_files(const Input& input)
{
    auto matching_name_regex{Tsepepe::utils::class_name_to_header_file_regex(input.class_name)};

    auto is_cpp_project_path{[](const fs::directory_entry& dir_entry) {
        // static constexpr auto ignored_file_patterns{std::experimental::make_array<std::string_view>(
        //     "tags", "compile_commands.json", ".*\\.log", ".*\\.swp", "*\\.swo", ".*\\.o", ".*\\.d"\\..*",
        //     "__pycache__/", "build.*/")};
        static constexpr auto ignored_directory_patterns{std::experimental::make_array<std::string_view>(
            // Any hidden directory.
            "\\..*",
            "__pycache__",
            "build.*")};

        if (not dir_entry.is_directory())
            return false;

        auto dir_path{dir_entry.path()};
        auto dir_name_str{std::prev(std::end(dir_path))->string()};
        return not std::ranges::any_of(ignored_directory_patterns, [&](const auto& pattern) {
            return std::regex_match(dir_name_str, std::regex(pattern.data()));
        });
    }};

    for (fs::recursive_directory_iterator it{input.project_root}; it != fs::recursive_directory_iterator(); ++it)
    {
        if (not is_cpp_project_path(*it))
        {
            it.disable_recursion_pending();
            continue;
        }
        std::cout << *it << std::endl;
    }
    // auto matching_by_name_header_view{ | std::views::filter(is_cpp_project_path)};
    // for (const auto& e : matching_by_name_header_view)
    // {
    //     std::cout << e << std::endl;
    //     is_cpp_project_path(e);
    // }
}

} // namespace Tsepepe::AbstractClassFinder


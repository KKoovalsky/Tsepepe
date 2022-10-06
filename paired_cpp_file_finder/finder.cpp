/**
 * @file	finder.cpp
 * @brief	Implements the paired C++ file finder core.
 */

#include <algorithm>
#include <ranges>

#include "finder.hpp"

namespace fs = std::filesystem;

// --------------------------------------------------------------------------------------------------------------------
// Helper declaration
// --------------------------------------------------------------------------------------------------------------------
static std::vector<fs::path> get_potential_paired_file_names(const fs::path&);

template<typename Range>
std::vector<fs::path> to_paths(Range& range)
{
    std::vector<fs::path> result;
    result.reserve(8);
    std::ranges::copy(range, std::back_inserter(result));
    return result;
}

// --------------------------------------------------------------------------------------------------------------------
// Helper variables
// --------------------------------------------------------------------------------------------------------------------
static const std::vector<std::string> source_file_extensions{".cpp", ".cxx", ".cc"};
static const std::vector<std::string> header_file_extensions{".hpp", ".hxx", ".h", ".hh"};

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::vector<std::filesystem::path> Tsepepe::PairedCppFileFinder::find(const Input& input)
{
    const auto& [project_root, cpp_file_path] = input;

    auto paired_file_names{get_potential_paired_file_names(cpp_file_path)};
    auto is_paired_cpp_file{[&](const fs::path& path) {
        return std::ranges::find(paired_file_names, path.filename()) != std::end(paired_file_names);
    }};

    auto cpp_file_dir{cpp_file_path.parent_path()};
    auto same_dir_matches_view{fs::directory_iterator{cpp_file_dir} | std::views::filter(is_paired_cpp_file)};
    auto same_dir_matches{to_paths(same_dir_matches_view)};
    if (not same_dir_matches.empty())
        return same_dir_matches;

    auto project_matches_view{fs::recursive_directory_iterator{project_root} | std::views::filter(is_paired_cpp_file)};
    auto project_matches{to_paths(project_matches_view)};
    return project_matches;
}

// --------------------------------------------------------------------------------------------------------------------
// Helper definition
// --------------------------------------------------------------------------------------------------------------------
static std::vector<fs::path> get_potential_paired_file_names(const fs::path& cpp_file_path)
{
    auto is_source_file{[](const fs::path& cpp_file_path) {
        return std::find(
                   std::begin(source_file_extensions), std::end(source_file_extensions), cpp_file_path.extension())
               != std::end(source_file_extensions);
    }};

    auto stem{cpp_file_path.stem()};
    const auto& extensions{is_source_file(cpp_file_path) ? source_file_extensions : header_file_extensions};

    std::vector<fs::path> result;
    result.reserve(extensions.size());

    std::ranges::transform(
        extensions, std::back_inserter(result), [&stem](const auto& ext) { return fs::path(stem).concat(ext); });

    return result;
}

/**
 * @file        utils.hpp
 * @brief       Declares the utilities.
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <string>

#include <clang/Tooling/CompilationDatabase.h>

#include "error.hpp"
#include "regex_wrapper.hpp"

namespace Tsepepe::utils
{

RegexWrapper class_name_to_header_file_regex(std::string class_name);

bool is_command_help_requested(int argc, const char** argv);

//! Throws if no compilation database is found under the specified directory, or when the directory doesn't exist.
std::unique_ptr<clang::tooling::CompilationDatabase>
parse_compilation_database(const std::filesystem::path& directory_with_compilation_database);

//! Throws when the path doesn't exist.
std::filesystem::path parse_and_validate_path(const std::filesystem::path& path);

}; // namespace Tsepepe::utils

#endif /* UTILS_HPP */

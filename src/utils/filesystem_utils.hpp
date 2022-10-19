/**
 * @file        filesystem_utils.hpp
 * @brief       Utilities for the filesystem.
 */
#ifndef FILESYSTEM_UTILS_HPP
#define FILESYSTEM_UTILS_HPP

#include <filesystem>

namespace Tsepepe::utils
{

//! Throws when the path doesn't exist.
std::filesystem::path parse_and_validate_path(const std::filesystem::path& path);

} // namespace Tsepepe::utils

#endif /* FILESYSTEM_UTILS_HPP */

/**
 * @file	filesystem_utils.cpp
 * @brief	Defines the filesystem utilities.
 */

#include "filesystem_utils.hpp"
#include "error.hpp"

namespace stdfs = std::filesystem;

namespace Tsepepe::utils::fs
{

std::filesystem::path parse_and_validate_path(const std::filesystem::path& path)
{
    if (not stdfs::exists(path))
    {
        std::string msg;
        auto path_as_string{path.string()};
        msg.reserve(40 + path_as_string.size());
        msg.append("ERROR: Path : ");
        msg.append(std::move(path_as_string));
        msg.append(" does not exist!");
        throw Tsepepe::Error{std::move(msg)};
    }

    return stdfs::absolute(path).lexically_normal();
}

} // namespace Tsepepe::utils::fs

/**
 * @file	filesystem_utils.cpp
 * @brief	Defines the filesystem utilities.
 */

#include "utils.hpp"

namespace fs = std::filesystem;

namespace Tsepepe::utils
{

std::filesystem::path parse_and_validate_path(const std::filesystem::path& path)
{
    if (not fs::exists(path))
    {
        std::string msg;
        auto path_as_string{path.string()};
        msg.reserve(40 + path_as_string.size());
        msg.append("ERROR: Path : ");
        msg.append(std::move(path_as_string));
        msg.append(" does not exist!");
        throw Tsepepe::Error{std::move(msg)};
    }

    return fs::absolute(path).lexically_normal();
}

} // namespace Tsepepe::utils

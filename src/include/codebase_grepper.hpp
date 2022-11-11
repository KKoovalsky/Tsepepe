/**
 * @file        codebase_grepper.hpp
 * @brief       Allows to grep over C++ files.
 */
#ifndef CODEBASE_GREPPER_HPP
#define CODEBASE_GREPPER_HPP

#include <filesystem>
#include <vector>

#include "common_types.hpp"

namespace Tsepepe
{

struct GrepMatch
{
    std::filesystem::path path;
    unsigned line;
    unsigned column;

    auto operator<=>(const GrepMatch&) const = default;
};

// FIXME: actually RustRegexPattern is used here!
std::vector<GrepMatch> codebase_grep(RootDirectory, EcmaScriptPattern);

} // namespace Tsepepe

#endif /* CODEBASE_GREPPER_HPP */

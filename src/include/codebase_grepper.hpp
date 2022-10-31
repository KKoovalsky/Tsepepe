/**
 * @file        codebase_grepper.hpp
 * @brief       Allows to grep over C++ files.
 */
#ifndef CODEBASE_GREPPER_HPP
#define CODEBASE_GREPPER_HPP

#include "NamedType/named_type_impl.hpp"
#include <filesystem>

#include <NamedType/named_type.hpp>

namespace Tsepepe
{

struct GrepMatch
{
    std::filesystem::path path;
    unsigned line;
    unsigned column;

    auto operator<=>(const GrepMatch&) const = default;
};

using EcmaScriptPattern = fluent::NamedType<std::string, struct PatternTag>;
using RootDirectory = fluent::NamedType<std::filesystem::path, struct RootDirectoryTag>;

std::vector<GrepMatch> codebase_grep(RootDirectory, EcmaScriptPattern);

} // namespace Tsepepe

#endif /* CODEBASE_GREPPER_HPP */

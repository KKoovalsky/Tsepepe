/**
 * @file        common_types.hpp
 * @brief       Common types definition.
 */
#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <filesystem>

#include <NamedType/named_type.hpp>

namespace Tsepepe
{

using RustRegexPattern = fluent::NamedType<std::string, struct RustRegexPatternTag>;
using EcmaScriptPattern = fluent::NamedType<std::string, struct EcmaScriptPatternTag>;
using RootDirectory = fluent::NamedType<std::filesystem::path, struct RootDirectoryTag>;

struct CodeInsertion
{
    std::string code;
    unsigned line{0};
    unsigned column{0};

    auto operator<=>(const CodeInsertion&) const = default;
};

} // namespace Tsepepe

#endif /* COMMON_TYPES_HPP */

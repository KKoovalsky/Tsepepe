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

using EcmaScriptPattern = fluent::NamedType<std::string, struct PatternTag>;
using RootDirectory = fluent::NamedType<std::filesystem::path, struct RootDirectoryTag>;

} // namespace Tsepepe

#endif /* COMMON_TYPES_HPP */

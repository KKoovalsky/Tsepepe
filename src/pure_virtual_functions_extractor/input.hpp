/**
 * @file        input.hpp
 * @brief       Input for the pure virtual functions extractor.
 */
#ifndef INPUT_HPP
#define INPUT_HPP

#include <filesystem>
#include <string>

#include <clang/Tooling/CompilationDatabase.h>

namespace Tsepepe::PureVirtualFunctionsExtractor
{

struct Input
{
    std::unique_ptr<clang::tooling::CompilationDatabase> compilation_database_ptr;
    std::filesystem::path header_file;
    std::string class_name;
};

} // namespace Tsepepe::PureVirtualFunctionsExtractor

#endif /* INPUT_HPP */

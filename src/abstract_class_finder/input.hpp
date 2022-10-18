/**
 * @file        input.hpp
 * @brief       Input for the abstract class finder.
 */
#ifndef INPUT_HPP
#define INPUT_HPP

#include <filesystem>
#include <memory>

#include <clang/Tooling/CompilationDatabase.h>

namespace Tsepepe::AbstractClassFinder
{

struct Input
{
    std::unique_ptr<clang::tooling::CompilationDatabase> compilation_database_ptr;
    std::filesystem::path project_root;
    std::string class_name;
};

} // namespace Tsepepe::AbstractClassFinder

#endif /* INPUT_HPP */

/**
 * @file        input.hpp
 * @brief       Defines input data for the suitable place in class finder.
 */
#ifndef INPUT_HPP
#define INPUT_HPP

#include <filesystem>
#include <string>

#include <clang/Tooling/CompilationDatabase.h>

namespace Tsepepe::SuitablePlaceInClassFinder
{

struct Input
{
    std::unique_ptr<clang::tooling::CompilationDatabase> compilation_database_ptr;
    std::filesystem::path header_file;
    std::string class_name;
};

}; // namespace Tsepepe::SuitablePlaceInClassFinder

#endif /* INPUT_HPP */

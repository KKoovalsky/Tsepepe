/**
 * @file        input.hpp
 * @brief       Defines input for the paired C++ file finder.
 */
#ifndef INPUT_HPP
#define INPUT_HPP

#include <filesystem>

namespace Tsepepe::PairedCppFileFinder
{

struct Input
{
    std::filesystem::path project_directory;
    std::filesystem::path cpp_file;
};

}; // namespace Tsepepe::PairedCppFileFinder

#endif /* INPUT_HPP */

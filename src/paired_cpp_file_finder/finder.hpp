/**
 * @file        finder.hpp
 * @brief       Declares the core of the paired C++ file finder.
 */
#ifndef FINDER_HPP
#define FINDER_HPP

#include <filesystem>
#include <vector>

#include "input.hpp"

namespace Tsepepe::PairedCppFileFinder
{

std::vector<std::filesystem::path> find(const Input&);

} // namespace Tsepepe::PairedCppFileFinder

#endif /* FINDER_HPP */

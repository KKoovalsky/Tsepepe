/**
 * @file        finder.hpp
 * @brief       The core of the suitable place in class finder.
 */
#ifndef FINDER_HPP
#define FINDER_HPP

#include <optional>
#include <string>

#include "input.hpp"

namespace Tsepepe::SuitablePlaceInClassFinder
{

/**
 * @returns Maybe a line with additional 'p' sign, which means that line 'public:' must be added below the line, and
 *          above the new content.
 */
std::optional<std::string> find(const Input&);

}; // namespace Tsepepe::SuitablePlaceInClassFinder

#endif /* FINDER_HPP */

/**
 * @file        finder.hpp
 * @brief       The core of the suitable place in class finder.
 */
#ifndef FINDER_HPP
#define FINDER_HPP

#include <optional>

#include "input.hpp"

namespace Tsepepe::SuitablePlaceInClassFinder
{

using Line = unsigned;
std::optional<Line> find(const Input&);

}; // namespace Tsepepe::SuitablePlaceInClassFinder

#endif /* FINDER_HPP */

/**
 * @file        extractor.hpp
 * @brief       The core of the pure virtual functions extractor.
 */
#ifndef EXTRACTOR_HPP
#define EXTRACTOR_HPP

#include <string>
#include <vector>

#include "input.hpp"

namespace Tsepepe::FullClassNameExpander
{

std::string expand(const Input& input);

}

#endif /* EXTRACTOR_HPP */

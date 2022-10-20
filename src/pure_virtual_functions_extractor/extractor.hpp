/**
 * @file        extractor.hpp
 * @brief       The core of the pure virtual functions extractor.
 */
#ifndef EXTRACTOR_HPP
#define EXTRACTOR_HPP

#include <string>
#include <vector>

#include "input.hpp"

namespace Tsepepe::PureVirtualFunctionsExtractor
{

std::vector<std::string> extract(const Input& input);

}

#endif /* EXTRACTOR_HPP */

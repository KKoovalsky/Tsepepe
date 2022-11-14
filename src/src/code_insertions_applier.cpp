/**
 * @file	code_insertions_applier.cpp
 * @brief	Implements applying of the insertions.
 */

#include "code_insertions_applier.hpp"

#include <algorithm>

std::string Tsepepe::apply_insertions(const std::string& input, std::vector<CodeInsertionByOffset> insertions)
{
    std::ranges::sort(insertions, [](const auto& l, const auto& r) { return l.offset < r.offset; });

    return {};
}

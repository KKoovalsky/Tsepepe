/**
 * @file        code_insertions_applier.hpp
 * @brief       Applies insertions to the code.
 */
#ifndef CODE_INSERTIONS_APPLIER_HPP
#define CODE_INSERTIONS_APPLIER_HPP

#include <string>
#include <vector>

#include "common_types.hpp"

namespace Tsepepe
{

std::string apply_insertions(const std::string& input, std::vector<CodeInsertionByOffset> insertions);

}

#endif /* CODE_INSERTIONS_APPLIER_HPP */

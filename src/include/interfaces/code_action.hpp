/**
 * @file        code_action.hpp
 * @brief       Interfaces for a code action.
 */
#ifndef CODE_ACTION_HPP
#define CODE_ACTION_HPP

#include <string>
#include <vector>

namespace Tsepepe
{

using Lines = std::vector<std::string>;

struct CodeAction
{
    virtual Lines apply(const Lines&) = 0;
    virtual ~CodeAction() = default;
};

} // namespace Tsepepe

#endif /* CODE_ACTION_HPP */

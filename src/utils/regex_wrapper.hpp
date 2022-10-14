/**
 * @file	regex_wrapper.hpp
 * @brief	Convenience regex wrapper which might be printed.
 */
#ifndef REGEX_WRAPPER_HPP
#define REGEX_WRAPPER_HPP

#include <regex>
#include <string>

namespace Tsepepe
{

struct RegexWrapper : std::regex, std::string
{
    RegexWrapper(std::string re) : std::regex{re}, std::string{std::move(re)}
    {
    }
};

} // namespace Tsepepe

#endif /* REGEX_WRAPPER_HPP */

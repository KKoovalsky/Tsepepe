/**
 * @file        string_utils.hpp
 * @brief       String utilities.
 */
#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <numeric>
#include <string>
#include <vector>

namespace Tsepepe::utils
{

template<typename BegIt, typename EndIt>
inline std::string join(BegIt begin, EndIt end, std::string delim)
{
    if (begin == end)
        return "";

    std::string init{*begin};
    init.reserve(120);
    return std::accumulate(
        std::next(begin), end, std::move(init), [&delim](std::string result, const std::string& elem) {
            return std::move(result) + delim + elem;
        });
}

inline std::string join(const std::vector<std::string>& string_vec, std::string delim = ", ")
{
    return join(std::begin(string_vec), std::end(string_vec), std::move(delim));
}

} // namespace Tsepepe::utils

#endif /* STRING_UTILS_HPP */

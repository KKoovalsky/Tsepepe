/**
 * @file        debug_utils.hpp
 * @brief       Debugging utilities.
 */
#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <iostream>
#include <iterator>

namespace Tsepepe
{

template<typename BeginIt, typename EndIt>
void dump_range(BeginIt begin, EndIt end, std::string dump_id = "dumping")
{
    std::cout << "Tsepepe: " << dump_id << ": " << std::endl;
    for (auto it{begin}; it != end; ++it)
        std::cout << "    " << *it << std::endl;
}

template<typename T>
void dump_container(const T& container, std::string dump_id = "dumping")
{
    dump_range(std::begin(container), std::end(container), dump_id);
}

}; // namespace Tsepepe

#endif /* DEBUG_UTILS_HPP */

/**
 * @file        error.hpp
 * @brief       Tsepepe Error definition.
 */
#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdexcept>

namespace Tsepepe
{
struct Error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
} // namespace Tsepepe

#endif /* ERROR_HPP */

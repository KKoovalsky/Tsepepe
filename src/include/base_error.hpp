/**
 * @file        base_error.hpp
 * @brief       Base error definition.
 */
#ifndef BASE_ERROR_HPP
#define BASE_ERROR_HPP

#include <stdexcept>

namespace Tsepepe
{

struct BaseError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

} // namespace Tsepepe

#endif /* BASE_ERROR_HPP */

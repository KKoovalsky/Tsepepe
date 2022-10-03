/**
 * @file        function_declaration_location.hpp
 * @brief       Defines the function info used by the function definition generator.
 */
#ifndef FUNCTION_DECLARATION_LOCATION_HPP
#define FUNCTION_DECLARATION_LOCATION_HPP

#include <string>

namespace Tsepepe
{

struct FunctionDeclarationLocation
{
    std::string file;
    unsigned line;
};

} // namespace Tsepepe

#endif /* FUNCTION_DECLARATION_LOCATION_HPP */

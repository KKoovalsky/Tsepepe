/**
 * @file        function_declaration_location.hpp
 * @brief       Defines the function info used by the function definition generator.
 */
#ifndef FUNCTION_DECLARATION_LOCATION_HPP
#define FUNCTION_DECLARATION_LOCATION_HPP

#include <string>

namespace CppTinyRefactor
{

struct FunctionDeclarationLocation
{
    std::string file;
    unsigned line;
};

} // namespace CppTinyRefactor

#endif /* FUNCTION_DECLARATION_LOCATION_HPP */

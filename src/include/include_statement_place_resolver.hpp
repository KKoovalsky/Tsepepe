/**
 * @file        include_statement_place_resolver.hpp
 * @brief
 */
#ifndef INCLUDE_STATEMENT_PLACE_RESOLVER_HPP
#define INCLUDE_STATEMENT_PLACE_RESOLVER_HPP

#include <string>

namespace Tsepepe
{

using Line = unsigned;

Line resolve_include_statement_place(const std::string& cpp_file_content);

} // namespace Tsepepe

#endif /* INCLUDE_STATEMENT_PLACE_RESOLVER_HPP */

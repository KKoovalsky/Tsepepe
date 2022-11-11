/**
 * @file        include_statement_place_resolver.hpp
 * @brief
 */
#ifndef INCLUDE_STATEMENT_PLACE_RESOLVER_HPP
#define INCLUDE_STATEMENT_PLACE_RESOLVER_HPP

#include <string>

namespace Tsepepe
{

struct IncludeStatementPlace
{
    unsigned line;
    bool is_newline_needed{false};

    constexpr auto operator<=>(const IncludeStatementPlace&) const = default;
};

IncludeStatementPlace resolve_include_statement_place(const std::string& cpp_file_content);

} // namespace Tsepepe

#endif /* INCLUDE_STATEMENT_PLACE_RESOLVER_HPP */

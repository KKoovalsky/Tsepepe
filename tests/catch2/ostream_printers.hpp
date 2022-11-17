/**
 * @file        ostream_printers.hpp
 * @brief       ostream operator overloads for the custom types.
 */
#ifndef OSTREAM_PRINTERS_HPP
#define OSTREAM_PRINTERS_HPP

#include <ostream>

#include "common_types.hpp"
#include "include_statement_place_resolver.hpp"

#include "libclang_utils/suitable_place_in_class_finder.hpp"

namespace Tsepepe
{

inline std::ostream& operator<<(std::ostream& os, const Tsepepe::CodeInsertionByOffset& code_insertion)
{
    return os << "Code: \"" << code_insertion.code << "\", offset: " << code_insertion.offset;
}

inline std::ostream& operator<<(std::ostream& os, const Tsepepe::IncludeStatementPlace& place)
{
    return os << "Offset: " << place.offset << ", is newline needed: " << place.is_newline_needed;
}

inline std::ostream& operator<<(std::ostream& os, const Tsepepe::SuitablePublicMethodPlaceInCppFile& place)
{
    return os << "Offset: " << place.offset << ", is public section needed: " << std::boolalpha
              << place.is_public_section_needed;
};

} // namespace Tsepepe

#endif /* OSTREAM_PRINTERS_HPP */

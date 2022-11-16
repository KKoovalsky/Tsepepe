/**
 * @file        finder.hpp
 * @brief       The core of the suitable place in class finder.
 */
#ifndef FINDER_HPP
#define FINDER_HPP

#include <string>

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

struct SuitablePublicMethodPlaceInCppFile
{
    unsigned offset;
    bool is_public_section_needed{false};

    auto operator<=>(const SuitablePublicMethodPlaceInCppFile&) const = default;
};

/**
 * @brief Finds a suitable place in class to put a public method.
 *
 * It analyzes the C++ file to find a proper place where a new public method may be put. Normally, it will return the
 * line which is just after the other explicit public methods, which are already declared, or defined. If no public
 * method is declared, or defined, then the first 'public' section line will be chosen. It means that it will be the
 * first line below the 'public:' access specifier. For structs, it will be the first line just after the body opening
 * '{' bracket. In case, no public section is defined within the class, then in the result the
 * SuitablePublicMethodPlaceInCppFile::is_public_section_needed will be set to true. This indicates that the class needs
 * to have 'public:' section added, just below the SuitablePublicMethodPlaceInCppFile::line number.
 */
SuitablePublicMethodPlaceInCppFile find_suitable_place_in_class_for_public_method(const std::string& cpp_file_content,
                                                                                  const clang::CXXRecordDecl*,
                                                                                  const clang::SourceManager&);

}; // namespace Tsepepe

#endif /* FINDER_HPP */

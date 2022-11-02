/**
 * @file        finder.hpp
 * @brief       The core of the suitable place in class finder.
 */
#ifndef FINDER_HPP
#define FINDER_HPP

#include "clang/Basic/LangOptions.h"
#include <filesystem>

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

struct SuitablePublicMethodPlaceInCppFile
{
    unsigned line;
    bool is_public_section_needed{false};

    auto operator<=>(const SuitablePublicMethodPlaceInCppFile&) const = default;
};

SuitablePublicMethodPlaceInCppFile find_suitable_place_in_class_for_public_method(std::filesystem::path cpp_file,
                                                                                  const clang::CXXRecordDecl*,
                                                                                  const clang::SourceManager&,
                                                                                  const clang::LangOptions&);

}; // namespace Tsepepe

#endif /* FINDER_HPP */

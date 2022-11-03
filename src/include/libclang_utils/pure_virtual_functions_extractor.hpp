/**
 * @file        pure_virtual_functions_extractor.hpp
 * @brief       Definition of a pure virtual functions extractor.
 */
#ifndef PURE_VIRTUAL_FUNCTIONS_EXTRACTOR_HPP
#define PURE_VIRTUAL_FUNCTIONS_EXTRACTOR_HPP

#include <string>
#include <vector>

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

using OverrideDeclarations = std::vector<std::string>;

OverrideDeclarations pure_virtual_functions_to_override_declarations(const clang::CXXRecordDecl*,
                                                                     const clang::SourceManager&);

} // namespace Tsepepe

#endif /* PURE_VIRTUAL_FUNCTIONS_EXTRACTOR_HPP */

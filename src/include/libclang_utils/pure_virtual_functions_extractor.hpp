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

/** @brief Extract all pure virtual functions from interface_node and turn then into override declarations.
 *
 * This will iterate over each method within the interface (pointed by the interface_node) and all its base classes
 * (and their base classes, and all their base classes, and so on ...) to look for a pure virtual functions. For each of
 * them the 'virtual' keyword, and the pure-specifier ("= 0"), will be deleted, and 'override' will be appended. All the
 * types (return types, types in parameters, nested templated types, ...) will be fully qualified, except those which
 * are defined within the interface (or which landed within it because of inheritance from its base classes), or share
 * common scope nesting with the implementor. The scope nesting of the implementor can be supplied with the
 * implementor_fully_qualified_name parameter.
 *
 * The fully qualified name is normally obtained from clang::CXXRecordDecl::getQualifiedNameAsString(). E.g. If the
 * class called Implementor, lays within a namespace called Namespace, then it's fully qualified name is
 * "Namespace::Implementor".
 *
 * @param interface_node Pointer to the definition of the interface from which the pure virtual functions will be
 *                       extracted.
 * @param implementor_fully_qualified_name Fully qualified name of the implementor. See above for explanation.
 * @returns The override declarations, ready to put straight into the implementor class, line by line.
 */
OverrideDeclarations pure_virtual_functions_to_override_declarations(const clang::CXXRecordDecl* interface_node,
                                                                     std::string implementor_fully_qualified_name,
                                                                     const clang::SourceManager&);

} // namespace Tsepepe

#endif /* PURE_VIRTUAL_FUNCTIONS_EXTRACTOR_HPP */

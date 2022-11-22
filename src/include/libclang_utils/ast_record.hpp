/**
 * @file        ast_record.hpp
 * @brief       AST record that bonds Clang AST context to a node.
 */
#ifndef AST_RECORD_HPP
#define AST_RECORD_HPP

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

template<typename Node>
struct ClangAstRecord
{
    const Node* node{nullptr};
    const clang::SourceManager* source_manager{nullptr};
};

using ClangClassRecord = ClangAstRecord<clang::CXXRecordDecl>;

} // namespace Tsepepe

#endif /* AST_RECORD_HPP */

/**
 * @file	base_specifier_resolver.cpp
 * @brief	Implements the base specifier resolver.
 */

#include "libclang_utils/base_specifier_resolver.hpp"

#include <clang/Lex/Lexer.h>

#include "base_error.hpp"
#include "scope_remover.hpp"

using namespace clang;
using namespace Tsepepe;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static SourceLocation get_end_of_token_before_opening_bracket(const CXXRecordDecl*, const SourceManager&);
static bool is_already_deriving(const CXXRecordDecl* potentially_deriving_class, const CXXRecordDecl* base_class);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
CodeInsertionByOffset Tsepepe::resolve_base_specifier(const std::string& cpp_file_content,
                                                      const clang::CXXRecordDecl* deriving_class,
                                                      const clang::CXXRecordDecl* base_class,
                                                      const clang::SourceManager& source_manager)
{
    if (is_already_deriving(deriving_class, base_class))
        return {};

    std::string code;
    code.reserve(80);

    if (deriving_class->bases().empty())
        code.append(" : ");
    else
        code.append(", ");

    if (not deriving_class->isStruct())
        code.append("public ");

    code.append(base_class->getQualifiedNameAsString());
    code = AllScopeRemover{FullyQualifiedName{deriving_class->getQualifiedNameAsString()}}.remove_from(code);

    auto placement{get_end_of_token_before_opening_bracket(deriving_class, source_manager)};
    auto offset{source_manager.getFileOffset(placement)};

    return {.code = std::move(code), .offset = offset};
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static SourceLocation get_end_of_token_before_opening_bracket(const CXXRecordDecl* record,
                                                              const SourceManager& source_manager)
{
    const auto& lang_opts{record->getLangOpts()};
    auto maybe_first_token{Lexer::findNextToken(record->getBeginLoc(), source_manager, lang_opts)};
    if (not maybe_first_token)
        throw BaseError{"Failed to get the first token after the beginning of class declaration!"};

    auto previous_token{*maybe_first_token};
    for (unsigned i{0}; i < 500; ++i)
    {
        auto maybe_next_token{Lexer::findNextToken(previous_token.getLocation(), source_manager, lang_opts)};
        if (not maybe_next_token)
            break;

        const auto& token{*maybe_next_token};
        if (token.getKind() == tok::TokenKind::l_brace)
            return previous_token.getEndLoc();

        previous_token = token;
    }

    throw BaseError{"Could not find the class' opening bracket!"};
}

static bool is_already_deriving(const CXXRecordDecl* potentially_deriving_class, const CXXRecordDecl* base_class)
{
    bool is_found{false};
    potentially_deriving_class->forallBases([&](const CXXRecordDecl* base) {
        if (base_class == base)
        {
            is_found = true;
            return false;
        }
        return true;
    });
    return is_found;
}

/**
 * @file        token_iterator.hpp
 * @brief       Token iterator that iterates over lexed range.
 */
#ifndef TOKEN_ITERATOR_HPP
#define TOKEN_ITERATOR_HPP

#include <iterator>

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

#include "base_error.hpp"

namespace Tsepepe
{

struct TokenIterator : std::forward_iterator_tag
{
    explicit TokenIterator(clang::SourceLocation location,
                           const clang::SourceManager* source_manager,
                           const clang::LangOptions* lang_options) :
        source_manager{source_manager}, lang_options{lang_options}
    {
        clang::Token token;
        if (clang::Lexer::getRawToken(location, token, *source_manager, *lang_options))
            throw Tsepepe::BaseError{
                "Couldn't create raw token for the location, while constructing the token iterator"};
        current_token = token;
    }

    TokenIterator() = default;
    TokenIterator(const TokenIterator&) = default;
    TokenIterator& operator=(const TokenIterator&) = default;
    TokenIterator(TokenIterator&&) = default;
    TokenIterator& operator=(TokenIterator&&) = default;

    using iterator_category = std::forward_iterator_tag;
    using difference_type = int;
    using value_type = clang::Token;
    using pointer = const value_type*;
    using reference = const value_type&;

    reference operator*() const
    {
        if (not current_token.has_value())
            throw Tsepepe::BaseError{"Dereferencing invalid token!"};
        return *current_token;
    }

    pointer operator->()
    {
        return &this->operator*();
    }

    //! Prefix increment
    TokenIterator& operator++()
    {
        if (not current_token.has_value())
            return *this;
        current_token = clang::Lexer::findNextToken(current_token->getLocation(), *source_manager, *lang_options);
        return *this;
    }

    //! Postfix increment
    TokenIterator operator++(int)
    {
        TokenIterator tmp{*this};
        ++(*this);
        return tmp;
    }

    friend bool operator==(const TokenIterator& a, const TokenIterator& b)
    {
        return a.current_token->getLocation() == b.current_token->getLocation();
    };

    friend bool operator!=(const TokenIterator& a, const TokenIterator& b)
    {
        return not(a == b);
    };

  private:
    clang::Optional<clang::Token> current_token;
    const clang::SourceManager* source_manager;
    const clang::LangOptions* lang_options;
};

static_assert(std::forward_iterator<TokenIterator>);

} // namespace Tsepepe

#endif /* TOKEN_ITERATOR_HPP */

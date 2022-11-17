/**
 * @file        lexed_range.hpp
 * @brief       A range that is lexed and can be iterated using TokenIterator.
 */
#ifndef LEXED_RANGE_HPP
#define LEXED_RANGE_HPP

#include "token_iterator.hpp"

namespace Tsepepe
{

struct LexedRange
{
    explicit LexedRange(clang::SourceLocation begin,
                        clang::SourceLocation end,
                        const clang::SourceManager* source_manager,
                        const clang::LangOptions* lang_options) :
        source_range{begin, end}, source_manager{source_manager}, lang_options{lang_options}
    {
    }

    explicit LexedRange(clang::SourceRange source_range,
                        const clang::SourceManager* source_manager,
                        const clang::LangOptions* lang_options) :
        source_range{std::move(source_range)}, source_manager{source_manager}, lang_options{lang_options}
    {
    }

    TokenIterator begin() const
    {
        return TokenIterator{source_range.getBegin(), source_manager, lang_options};
    }

    TokenIterator end() const
    {
        return TokenIterator{source_range.getEnd(), source_manager, lang_options};
    }

  private:
    clang::SourceRange source_range;
    const clang::SourceManager* source_manager;
    const clang::LangOptions* lang_options;
};

static_assert(std::ranges::forward_range<LexedRange>);

} // namespace Tsepepe

#endif /* LEXED_RANGE_HPP */

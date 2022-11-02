/**
 * @file        presumed_source_range.hpp
 * @brief       Presumed source range wrapper.
 */
#ifndef PRESUMED_SOURCE_RANGE_HPP
#define PRESUMED_SOURCE_RANGE_HPP

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

namespace Tsepepe
{

struct PresumedSourceRange
{
    PresumedSourceRange() :
        begin{nullptr, clang::FileID{}, 0, 0, clang::SourceLocation{}},
        end{nullptr, clang::FileID{}, 0, 0, clang::SourceLocation{}}
    {
    }

    PresumedSourceRange(const clang::SourceManager& source_manager,
                        const clang::SourceLocation& begin_,
                        const clang::SourceLocation& end_) :
        begin{source_manager.getPresumedLoc(begin_)}, end{source_manager.getPresumedLoc(end_)}
    {
    }

    bool is_surrounding(const PresumedSourceRange& potential_inner_source_range) const
    {
        // If I am invalid, then assume the other source range is within.
        if (begin.isInvalid() or end.isInvalid())
            return true;

        const auto& other{potential_inner_source_range};

        // Analogous to above, but the opposite case.
        if (other.begin.isInvalid() or other.end.isInvalid())
            return false;

        if (begin.getFileID() != begin.getFileID())
            return false;

        auto begin_line{begin.getLine()}, end_line{end.getLine()};
        auto other_begin_line{other.begin.getLine()}, other_end_line{other.end.getLine()};
        auto begin_column{begin.getColumn()}, end_column{end.getColumn()};
        auto other_begin_column{other.begin.getColumn()}, other_end_column{other.end.getColumn()};

        auto is_other_begin_further{begin_line < other_begin_line //
                                    or (begin_line == other_begin_line and begin_column <= other_begin_column)};
        auto is_other_end_closer{end_line > other_end_line //
                                 or (end_line == other_end_line and end_column >= other_end_column)};
        return is_other_begin_further and is_other_end_closer;
    }

    bool is_line_inside(unsigned line_number)
    {
        return begin.getLine() >= line_number and line_number <= end.getLine();
    }

  private:
    clang::PresumedLoc begin;
    clang::PresumedLoc end;
};

} // namespace Tsepepe

#endif /* PRESUMED_SOURCE_RANGE_HPP */

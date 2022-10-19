/**
 * @file        fs_ranges_utils.hpp
 * @brief       Utilities wrapping std::filesystem: iterator, range and view allowing ignoring certain directories,
 *              while iterating recursively over filesystem.
 */
#ifndef FS_RANGES_UTILS_HPP
#define FS_RANGES_UTILS_HPP

#include <algorithm>
#include <filesystem>
#include <functional>
#include <ranges>

namespace Tsepepe::utils::fs
{

namespace fs = std::filesystem;
namespace ranges = std::ranges;

using RecursiveFsIt = fs::recursive_directory_iterator;

using DirectoryEntryMatchPredicate = std::function<bool(const fs::directory_entry&)>;

struct IgnoringRecursiveDirectoryIterator : fs::recursive_directory_iterator
{
    explicit IgnoringRecursiveDirectoryIterator(fs::recursive_directory_iterator it,
                                                DirectoryEntryMatchPredicate pred) :
        fs::recursive_directory_iterator(it), predicate{std::move(pred)}
    {
        advance_to_next_match();
    }

    IgnoringRecursiveDirectoryIterator& operator++()
    {
        fs::recursive_directory_iterator::operator++();
        advance_to_next_match();
        return *this;
    }

    IgnoringRecursiveDirectoryIterator operator++(int)
    {
        auto r{*this};
        ++*this;
        return r;
    }

  private:
    void advance_to_next_match()
    {
        auto& self{*this};
        while (self != fs::recursive_directory_iterator{})
        {
            if (const auto& dir_entry{*self}; not predicate(dir_entry))
            {
                if (fs::is_directory(dir_entry))
                    self.disable_recursion_pending();
            } else
            {
                break;
            }
            fs::recursive_directory_iterator::operator++();
        }
    }

    DirectoryEntryMatchPredicate predicate;
};

class directories_filter_view : public ranges::view_interface<directories_filter_view>
{
  public:
    directories_filter_view(RecursiveFsIt it, DirectoryEntryMatchPredicate pred) : it{std::move(it), std::move(pred)}
    {
    }

    IgnoringRecursiveDirectoryIterator base() const&
    {
        return it;
    }

    IgnoringRecursiveDirectoryIterator base() &&
    {
        return std::move(it);
    }

    auto begin() const
    {
        return it;
    }

    auto end() const
    {
        return RecursiveFsIt{};
    }

  private:
    IgnoringRecursiveDirectoryIterator it;
};

namespace detail
{

struct directories_filter_range_adaptor_closure
{
    directories_filter_range_adaptor_closure(DirectoryEntryMatchPredicate pred) : predicate{std::move(pred)}
    {
    }

    auto operator()(RecursiveFsIt it) const
    {
        return directories_filter_view(std::move(it), predicate);
    }

  private:
    const DirectoryEntryMatchPredicate predicate;
};

struct directories_filter_range_adaptor
{
    auto operator()(RecursiveFsIt it, DirectoryEntryMatchPredicate pred)
    {
        return directories_filter_view(std::move(it), std::move(pred));
    }

    auto operator()(DirectoryEntryMatchPredicate pred)
    {
        return directories_filter_range_adaptor_closure(std::move(pred));
    }
};

auto operator|(RecursiveFsIt it, const directories_filter_range_adaptor_closure& closure)
{
    return closure(std::move(it));
}
}; // namespace detail

namespace views
{
detail::directories_filter_range_adaptor directories_filter;
}

template<typename Range>
std::vector<fs::path> to_paths(Range& range)
{
    std::vector<fs::path> result;
    result.reserve(8);
    std::ranges::copy(range, std::back_inserter(result));
    return result;
}

} // namespace Tsepepe::utils::fs

#endif /* FS_RANGES_UTILS_HPP */

/**
 * @file	code_insertions_applier.cpp
 * @brief	Implements applying of the insertions.
 */

#include "code_insertions_applier.hpp"
#include "base_error.hpp"

#include <algorithm>
#include <numeric>
#include <string>

// --------------------------------------------------------------------------------------------------------------------
// Private types and aliases
// --------------------------------------------------------------------------------------------------------------------
using CodeInsertions = std::vector<Tsepepe::CodeInsertionByOffset>;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static void validate_insertions_in_bounds(const std::string& input, const CodeInsertions&);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::string Tsepepe::apply_insertions(const std::string& input, std::vector<CodeInsertionByOffset> insertions)
{
    validate_insertions_in_bounds(input, insertions);

    auto [new_insertions_end_it, insertions_end_it] = std::ranges::remove_if(
        insertions, [](const CodeInsertionByOffset& insertion) { return insertion.code.empty(); });
    insertions.erase(new_insertions_end_it, insertions_end_it);

    std::ranges::sort(insertions, [](const auto& l, const auto& r) { return l.offset < r.offset; });

    std::vector<std::string_view> chunks;
    chunks.reserve(insertions.size() + 1);

    auto chunk_begin_it{std::begin(input)};
    for (const auto& insertion : insertions)
    {
        auto chunk_end_it{std::next(std::begin(input), insertion.offset)};
        chunks.emplace_back(std::string_view(chunk_begin_it, chunk_end_it));
        chunk_begin_it = chunk_end_it;
    }
    chunks.emplace_back(std::string_view(chunk_begin_it, std::end(input)));

    auto insertions_total_size{std::accumulate(
        std::begin(insertions), std::end(insertions), 0u, [](auto sum, const CodeInsertionByOffset& insertion) {
            return sum + insertion.code.size();
        })};

    std::string result;
    result.reserve(insertions_total_size + input.size());
    for (unsigned i{0}; i < insertions.size(); ++i)
    {
        result += chunks[i];
        result += std::move(insertions[i].code);
    }
    result += chunks.back();

    return result;
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static void validate_insertions_in_bounds(const std::string& input, const CodeInsertions& insertions)
{
    for (const auto& insertion : insertions)
        if (input.size() < insertion.offset)
            throw Tsepepe::BaseError{"Code insertion: \"" + insertion.code + "\", at offset "
                                     + std::to_string(insertion.offset) + ", out of bounds"};
}

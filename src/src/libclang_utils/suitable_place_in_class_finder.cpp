/**
 * @file	finder.cpp
 * @brief	Implements the core of the suitable place in class finder.
 */

#include "libclang_utils/suitable_place_in_class_finder.hpp"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "base_error.hpp"
#include "common_types.hpp"
#include "file_grepper.hpp"

using namespace clang;
using namespace Tsepepe;

#include "libclang_utils/lexed_range.hpp"
#include "libclang_utils/misc_utils.hpp"

// --------------------------------------------------------------------------------------------------------------------
// Private stuff
// --------------------------------------------------------------------------------------------------------------------
struct SuitablePlaceInClassFinder
{
    //! Beware: None of the parameters are owned, thus they must outlive the SuitablePlaceInClassFinder.
    explicit SuitablePlaceInClassFinder(const std::string& cpp_file_content,
                                        const clang::CXXRecordDecl* node,
                                        const clang::SourceManager& source_manager) :
        cpp_file_content{cpp_file_content},
        record{node},
        source_manager{source_manager},
        lang_options{node->getLangOpts()}
    {
        if (not node->isThisDeclarationADefinition())
            throw Tsepepe::BaseError{"Class definition required for the SuitablePlaceInClassFinder"};
    }

    SuitablePublicMethodPlaceInCppFile find() const
    {
        SourceLocation result;
        bool is_public_section_needed{false};

        if (auto maybe_after_public_method{find_location_after_last_public_method_in_the_first_chain()};
            maybe_after_public_method)
            result = *maybe_after_public_method;
        else if (auto maybe_within_public_section{find_first_public_access_modifier_end_location()};
                 maybe_within_public_section)
            result = *maybe_within_public_section;
        else
        {
            result = get_class_opening_bracket_source_location();
            is_public_section_needed = not record->isStruct();
        }

        return {.offset = get_insert_offset_after_location(result),
                .is_public_section_needed = is_public_section_needed};
    }

  private:
    std::optional<SourceLocation> find_location_after_last_public_method_in_the_first_chain() const
    {
        auto is_public_explicit_method{[](const CXXMethodDecl* method) {
            return method->getAccess() == AccessSpecifier::AS_public and not method->isImplicit();
        }};

        auto is_not_public_or_is_public_implicit_method{[&](const CXXMethodDecl* method) {
            return not is_public_explicit_method(method);
        }};

        auto beg{record->method_begin()};
        auto end{record->method_end()};

        auto first_public_method_it{std::ranges::find_if(beg, end, is_public_explicit_method)};
        if (first_public_method_it == end)
            return {};

        // Ideally, we could use find_if() to find the first non-public method, and then use std::prev(), but the method
        // range is a forward range.
        auto last_public_method_in_first_public_method_chain_it{first_public_method_it};
        for (auto it{first_public_method_it}; it != end; ++it)
        {
            if (is_not_public_or_is_public_implicit_method(*it))
                break;
            last_public_method_in_first_public_method_chain_it = it;
        }

        auto result{last_public_method_in_first_public_method_chain_it};
        if (not result->isThisDeclarationADefinition())
        {
            LexedRange code_range{result->getEndLoc(), record->getEndLoc(), &source_manager, &record->getLangOpts()};
            auto semi_it{std::find_if(code_range.begin(), code_range.end(), is_semicolon)};
            return semi_it->getLocation();
        } else
        {
            return result->getEndLoc();
        }
    }

    std::optional<SourceLocation> find_first_public_access_modifier_end_location() const
    {
        auto class_lexed_range{make_class_lexed_range()};
        auto beg{class_lexed_range.begin()};
        auto end{class_lexed_range.end()};

        auto it{std::find_if(beg, end, [](const Token& token) {
            return token.is(tok::raw_identifier) and token.getRawIdentifier() == "public";
        })};

        if (it == end)
            return {};

        it = std::find_if(it, end, [](const Token& token) { return token.is(tok::colon); });
        if (it == end)
            return {};

        return it->getLocation();
    }

    unsigned get_insert_offset_after_location(SourceLocation location) const
    {
        LexedRange code_range{location, record->getEndLoc(), &source_manager, &lang_options};
        auto beg{code_range.begin()};
        auto end{code_range.end()};

        // Assume the 'location' points to the at-the-end location, not to the past-the-end location, thus
        // incrementation will put us to the past-the-end location.
        ++beg;

        // Find any token which is 'not-transparent' for the parser
        auto it{std::find_if_not(beg, end, is_semicolon)};

        auto beg_offset{source_manager.getFileOffset(code_range.begin()->getLocation())};
        auto offset{source_manager.getFileOffset(it->getLocation())};
        auto newline_offset{cpp_file_content.find('\n', beg_offset)};

        if (newline_offset != std::string::npos and newline_offset <= offset)
            return newline_offset + 1;
        else
            return source_manager.getFileOffset(beg->getLocation());
    }

    SourceLocation unpack_source_location_from_token(const Optional<Token>& token) const
    {
        if (not token.has_value())
            throw Tsepepe::BaseError{"Can't unpack token; token empty!"};

        auto loc{token->getLocation()};
        if (not loc.isValid())
            throw Tsepepe::BaseError{"Can't unpack token; token holds invalid location!"};
        return loc;
    }

    SourceLocation get_class_opening_bracket_source_location() const
    {
        auto class_lexed_range{make_class_lexed_range()};
        auto it{std::find_if(class_lexed_range.begin(), class_lexed_range.end(), [](const Token& token) {
            return token.is(tok::l_brace);
        })};
        return it->getLocation();
    }

    static bool is_semicolon(const Token& token)
    {
        return token.is(tok::semi);
    }

    LexedRange make_class_lexed_range() const
    {
        return LexedRange{record->getSourceRange(), &source_manager, &record->getLangOpts()};
    }

    const std::string& cpp_file_content;
    const CXXRecordDecl* record;
    const SourceManager& source_manager;
    const LangOptions& lang_options;
};

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::SuitablePublicMethodPlaceInCppFile Tsepepe::find_suitable_place_in_class_for_public_method(
    const std::string& cpp_file_content, const clang::CXXRecordDecl* node, const clang::SourceManager& source_manager)
{
    return SuitablePlaceInClassFinder{cpp_file_content, node, source_manager}.find();
}

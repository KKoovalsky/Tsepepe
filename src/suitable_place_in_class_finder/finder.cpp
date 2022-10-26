/**
 * @file	finder.cpp
 * @brief	Implements the core of the suitable place in class finder.
 */
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <boost/process.hpp>

#include "finder.hpp"

#include "clang_ast_utils.hpp"

using namespace clang;
using namespace clang::tooling;
namespace ast = clang::ast_matchers;

namespace fs = std::filesystem;

// --------------------------------------------------------------------------------------------------------------------
// Private stuff
// --------------------------------------------------------------------------------------------------------------------
AST_MATCHER_P(CXXRecordDecl, isMemberOf, std::string, name)
{
    return false;
};

class LineFinder : public ast::MatchFinder::MatchCallback
{
  public:
    LineFinder(fs::path header_file) : printing_policy{lang_options}, header_file{std::move(header_file)}
    {
        printing_policy.adjustForCPlusPlus();
    }

    void run(const ast::MatchFinder::MatchResult& result) override
    {
        auto node{result.Nodes.getNodeAs<CXXRecordDecl>("class")};
        if (node == nullptr or not node->hasDefinition())
            return;

        const auto& source_manager{result.Context->getSourceManager()};

        auto last_public_method_in_first_public_method_chain{find_last_public_method_in_first_public_chain(node)};
        if (last_public_method_in_first_public_method_chain != nullptr)
        {
            auto past_end_source_loc{last_public_method_in_first_public_method_chain->getEndLoc()};
            auto end_source_loc{Lexer::getLocForEndOfToken(past_end_source_loc, 0, source_manager, lang_options)};
            auto r{source_manager.getSpellingLineNumber(end_source_loc)};
            found_line_number = r;
        } else if (auto maybe_first_public_section{try_find_line_with_public_section(node, source_manager)};
                   maybe_first_public_section)
        {
            found_line_number = *maybe_first_public_section;
        } else
        {
            found_line_number = find_line_with_opening_bracket(node, source_manager);
            is_public_section_needed = true;
        }
    }

    std::optional<std::string> get_result() const
    {
        if (!found_line_number)
            return {};

        std::string r;
        r.reserve(16);
        r = std::to_string(*found_line_number);

        if (is_public_section_needed)
            r.append("p");

        return r;
    }

  private:
    const CXXMethodDecl* find_last_public_method_in_first_public_chain(const CXXRecordDecl* record) const
    {
        auto is_public_method{[](const CXXMethodDecl* method) {
            return method->getAccess() == AccessSpecifier::AS_public;
        }};

        auto is_not_public_method{[&](const CXXMethodDecl* method) {
            return not is_public_method(method);
        }};

        auto beg{record->method_begin()};
        auto end{record->method_end()};

        auto first_public_method_it{std::ranges::find_if(beg, end, is_public_method)};
        if (first_public_method_it == end)
            return nullptr;

        // Ideally, we could use find_if() to find the first non-public method, and then use std::prev(), but the method
        // range is a forward range.
        auto last_public_method_in_first_public_method_chain_it{first_public_method_it};
        for (auto it{first_public_method_it}; it != end; ++it)
        {
            if (is_not_public_method(*it) or it->isImplicit())
                break;
            last_public_method_in_first_public_method_chain_it = it;
        }

        return *last_public_method_in_first_public_method_chain_it;
    }

    std::optional<unsigned> try_find_line_with_public_section(const CXXRecordDecl* record,
                                                              const SourceManager& source_manager) const
    {
        auto get_class_body_begin_end_lines{[&]() {
            auto class_body_source_range{record->getSourceRange()};
            auto class_body_past_end_loc{class_body_source_range.getEnd()};
            auto class_body_end_loc{
                Lexer::getLocForEndOfToken(class_body_past_end_loc, 0, source_manager, lang_options)};
            auto class_body_begin_line{source_manager.getSpellingLineNumber(class_body_source_range.getBegin())};
            auto class_body_end_line{source_manager.getSpellingLineNumber(class_body_end_loc)};
            return std::make_pair(class_body_begin_line, class_body_end_line);
        }};

        auto get_line_nums_with_public_access_specifier_declarations{[&]() -> std::vector<unsigned> {
            std::string command{"rg \"public\\s*:\" " + header_file.string()};

            using namespace boost::process;
            ipstream pipe_stream;
            child c{std::move(command), std_out > pipe_stream};
            std::string line;

            std::vector<unsigned> result;
            result.reserve(16);

            while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
            {
                auto colon_idx{line.find(':')};
                if (colon_idx == std::string::npos)
                    continue;
                auto line_number_str{line.substr(0, colon_idx)};
                result.emplace_back(std::stoul(line_number_str));
            }
            c.wait();

            return result;
        }};

        auto class_body_line_range{get_class_body_begin_end_lines()};
        auto is_line_within_class_body{[&](unsigned line) {
            return line >= class_body_line_range.first and line <= class_body_line_range.second;
        }};

        auto lines_with_public_access_specifier_declarations{get_line_nums_with_public_access_specifier_declarations()};
        auto public_section_within_the_class_it{
            std::ranges::find_if(lines_with_public_access_specifier_declarations, is_line_within_class_body)};
        if (public_section_within_the_class_it != std::end(lines_with_public_access_specifier_declarations))
            return *public_section_within_the_class_it;
        else
            return {};
    }

    unsigned find_line_with_opening_bracket(const CXXRecordDecl* record, const SourceManager& source_manager) const
    {
        auto location{record->getBeginLoc()};
        while (true)
        {
            auto maybe_token{Lexer::findNextToken(location, source_manager, lang_options)};
            if (not maybe_token)
                break;

            const auto& token{*maybe_token};
            location = token.getLocation();
            if (token.getKind() == tok::TokenKind::l_brace)
                break;
        }

        return source_manager.getSpellingLineNumber(location);
    }

    std::optional<unsigned> found_line_number;
    bool is_public_section_needed{false};

    clang::LangOptions lang_options;
    clang::PrintingPolicy printing_policy;
    fs::path header_file;
};

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::optional<std::string> Tsepepe::SuitablePlaceInClassFinder::find(const Input& input)
{
    auto class_matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasName(input.class_name)).bind("class")};

    ast::MatchFinder finder;
    LineFinder line_finder{input.header_file};
    finder.addMatcher(class_matcher, &line_finder);

    ClangTool tool{*input.compilation_database_ptr, {input.header_file}};

    IgnoringDiagConsumer diagnostic_consumer;
    tool.setDiagnosticConsumer(&diagnostic_consumer);

    tool.run(newFrontendActionFactory(&finder).get());

    return line_finder.get_result();
}

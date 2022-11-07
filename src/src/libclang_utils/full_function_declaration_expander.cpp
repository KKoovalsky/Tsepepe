/**
 * @file	full_function_declaration_expander.cpp
 * @brief	Implements the full expanding of the the function declaration.
 */
#include <algorithm>
#include <iterator>
#include <numeric>

#include "libclang_utils/full_function_declaration_expander.hpp"

#include <clang/AST/Attr.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>

#include "libclang_utils/misc_utils.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Templates
// --------------------------------------------------------------------------------------------------------------------
template<typename BegIt, typename EndIt>
static std::string join(BegIt begin, EndIt end, std::string delim)
{
    if (begin == end)
        return "";

    std::string init{*begin};
    init.reserve(120);
    return std::accumulate(
        std::next(begin), end, std::move(init), [&delim](std::string result, const std::string& elem) {
            return std::move(result) + delim + elem;
        });
}

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static std::string join(const std::vector<std::string>& string_vec, std::string delim = ", ");

// --------------------------------------------------------------------------------------------------------------------
// Private helper types
// --------------------------------------------------------------------------------------------------------------------
namespace FullFunctionDeclarationExpander
{

struct Expander
{
    explicit Expander(const SourceManager& sm, Tsepepe::FullFunctionDeclarationExpanderOptions opts) :
        printing_policy{lang_options}, source_manager{sm}, options{std::move(opts)}
    {
        printing_policy.adjustForCPlusPlus();
    }

    std::string expand(const FunctionDecl* function) const
    {
        std::vector<std::string> result_parted;
        result_parted.reserve(8);

        if (not options.ignore_attribute_specifiers)
            result_parted.emplace_back(get_standard_attributes(function));

        result_parted.emplace_back(get_return_type(function));
        result_parted.emplace_back(function->getQualifiedNameAsString() + get_parameters(function));

        if (auto method{dynamic_cast<const CXXMethodDecl*>(function)}; method != nullptr)
        {
            result_parted.emplace_back(method->isConst() ? "const" : "");
            result_parted.emplace_back(get_ref_qualifier(method));
            result_parted.emplace_back(get_noexcept_qualifier(method));
        }

        auto [end, _] = std::ranges::remove_if(result_parted, [](const auto& s) { return s.empty(); });
        auto begin{std::begin(result_parted)};
        return join(begin, end, " ");
    }

  private:
    std::string get_standard_attributes(const FunctionDecl* node) const
    {
        std::vector<std::string> standard_attributes_as_strings;
        standard_attributes_as_strings.reserve(4);

        auto attrs{node->getAttrs()};
        for (const auto& attr : attrs)
        {
            if (attr->isStandardAttributeSyntax())
            {
                auto attribute_as_string{
                    "[["                                                                                      //
                    + Tsepepe::source_range_content_to_string(attr->getRange(), source_manager, lang_options) //
                    + "]]"};
                standard_attributes_as_strings.emplace_back(std::move(attribute_as_string));
            }
        }

        return join(standard_attributes_as_strings, " ");
    }

    std::string get_return_type(const FunctionDecl* node) const
    {
        auto has_explicit_return_type{[&](const FunctionDecl* node) {
            auto return_type_as_written_in_code{Tsepepe::source_range_content_to_string(
                node->getReturnTypeSourceRange(), source_manager, node->getLangOpts())};
            return not return_type_as_written_in_code.empty();
        }};

        if (not has_explicit_return_type(node))
            return "";

        auto ret_qual_type{node->getReturnType()};
        auto ret_type{ret_qual_type.getTypePtr()};
        if (auto template_spec_type{ret_type->getAs<TemplateSpecializationType>()}; template_spec_type != nullptr)
            return stringify_template_specialization(template_spec_type);

        return ret_qual_type.getAsString(printing_policy);
    }

    std::string stringify_template_specialization(const TemplateSpecializationType* template_spec_type) const
    {
        auto template_arg_to_string{[&](const TemplateArgument& template_arg) {
            std::string s;
            llvm::raw_string_ostream os{s};
            template_arg.print(printing_policy, os, true);
            return s;
        }};

        std::string result;
        result.reserve(100);

        llvm::raw_string_ostream os{result};

        auto template_name{template_spec_type->getTemplateName()};
        template_name.print(os, printing_policy, TemplateName::Qualified::Fully);

        const auto& template_args{template_spec_type->template_arguments()};
        std::vector<std::string> template_args_as_string;
        template_args_as_string.reserve(template_args.size());
        std::ranges::transform(template_args, std::back_inserter(template_args_as_string), template_arg_to_string);

        os << '<' << join(template_args_as_string) << '>';

        return result;
    }

    std::string get_parameters(const FunctionDecl* node) const
    {
        auto param_to_string{[&](const ParmVarDecl* param) {
            auto result{param->getType().getAsString(printing_policy)};
            auto name{param->getQualifiedNameAsString()};
            if (not name.empty())
            {
                result += ' ';
                result += std::move(name);
            }
            return result;
        }};

        const auto& params{node->parameters()};
        std::vector<std::string> params_as_string;
        params_as_string.reserve(params.size());
        std::ranges::transform(params, std::back_inserter(params_as_string), param_to_string);

        return '(' + join(params_as_string) + ')';
    }

    std::string get_ref_qualifier(const CXXMethodDecl* node) const
    {
        auto ref_qualifier{node->getRefQualifier()};
        if (ref_qualifier == RefQualifierKind::RQ_LValue)
            return "&";
        else if (ref_qualifier == RefQualifierKind::RQ_RValue)
            return "&&";
        return "";
    }

    std::string get_noexcept_qualifier(const CXXMethodDecl* node) const
    {
        auto source_range{node->getExceptionSpecSourceRange()};
        if (source_range.isInvalid())
            return "";

        return Tsepepe::source_range_content_to_string(source_range, source_manager, node->getLangOpts());
    }

    static inline const LangOptions lang_options;
    PrintingPolicy printing_policy{lang_options};
    const SourceManager& source_manager;
    const Tsepepe::FullFunctionDeclarationExpanderOptions options;
};

} // namespace FullFunctionDeclarationExpander

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::string Tsepepe::fully_expand_function_declaration(const FunctionDecl* function,
                                                       const SourceManager& source_manager,
                                                       FullFunctionDeclarationExpanderOptions options)
{
    return FullFunctionDeclarationExpander::Expander{source_manager, std::move(options)}.expand(function);
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static std::string join(const std::vector<std::string>& string_vec, std::string delim)
{
    return join(std::begin(string_vec), std::end(string_vec), std::move(delim));
}


/**
 * @file	full_function_declaration_expander.cpp
 * @brief	Implements the full expanding of the the function declaration.
 */
#include "libclang_utils/full_function_declaration_expander.hpp"

#include <clang/AST/Attr.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>

#include <algorithm>
#include <iterator>
#include <numeric>

#include "libclang_utils/misc_utils.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static std::string get_standard_attributes(const FunctionDecl*, const SourceManager&);
static std::string get_return_type(const FunctionDecl*, const SourceManager&, const PrintingPolicy&);
static std::string stringify_template_specialization(const TemplateSpecializationType*, const PrintingPolicy&);
static std::string get_parameters(const FunctionDecl*, const SourceManager&, const PrintingPolicy&);
static std::string join(const std::vector<std::string>& string_vec, std::string delim = ", ");

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::string Tsepepe::fully_expand_function_declaration(const FunctionDecl* function,
                                                       const SourceManager& source_manager,
                                                       FullFunctionDeclarationExpanderOptions options)
{
    std::string result;
    result.reserve(120);

    LangOptions lang_opts;
    PrintingPolicy printing_policy{lang_opts};
    printing_policy.adjustForCPlusPlus();

    if (not options.ignore_attribute_specifiers)
    {
        auto standard_attributes_stringified{get_standard_attributes(function, source_manager)};
        if (not standard_attributes_stringified.empty())
        {
            result.append(std::move(standard_attributes_stringified));
            result += ' ';
        }
    }

    auto return_type_as_string{get_return_type(function, source_manager, printing_policy)};
    if (not return_type_as_string.empty())
    {
        result.append(std::move(return_type_as_string));
        result += ' ';
    }

    result.append(function->getQualifiedNameAsString());
    result.append(get_parameters(function, source_manager, printing_policy));

    return result;
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static std::string get_standard_attributes(const FunctionDecl* node, const SourceManager& source_manager)
{
    std::vector<std::string> standard_attributes_as_strings;
    standard_attributes_as_strings.reserve(4);

    auto attrs{node->getAttrs()};
    for (const auto& attr : attrs)
    {
        if (attr->isStandardAttributeSyntax())
        {
            auto attribute_as_string{
                "[["                                                                                             //
                + Tsepepe::source_range_content_to_string(attr->getRange(), source_manager, node->getLangOpts()) //
                + "]]"};
            standard_attributes_as_strings.emplace_back(std::move(attribute_as_string));
        }
    }

    return join(standard_attributes_as_strings, " ");
}

static std::string
get_return_type(const FunctionDecl* node, const SourceManager& source_manager, const PrintingPolicy& printing_policy)
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
        return stringify_template_specialization(template_spec_type, printing_policy);

    return ret_qual_type.getAsString(printing_policy);
}

static std::string stringify_template_specialization(const TemplateSpecializationType* template_spec_type,
                                                     const PrintingPolicy& printing_policy)
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

static std::string
get_parameters(const FunctionDecl* node, const SourceManager& source_manager, const PrintingPolicy& printing_policy)
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

static std::string join(const std::vector<std::string>& string_vec, std::string delim)
{
    if (string_vec.size() == 0)
        return "";

    std::string init{string_vec[0]};
    init.reserve(120);
    return std::accumulate(
        std::next(std::begin(string_vec)),
        std::end(string_vec),
        std::move(init),
        [&delim](std::string result, const std::string& elem) { return std::move(result) + delim + elem; });
}


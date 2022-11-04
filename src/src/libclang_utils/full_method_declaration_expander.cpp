/**
 * @file	full_method_declaration_expander.cpp
 * @brief	Implements the full expanding of the the method declaration.
 */
#include "libclang_utils/full_method_declaration_expander.hpp"

#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>

#include "libclang_utils/misc_utils.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------
static std::string get_return_type(const CXXMethodDecl*, const SourceManager&, const PrintingPolicy&);
static std::string stringify_template_specialization(const TemplateSpecializationType*, const PrintingPolicy&);
static std::string get_parameters(const CXXMethodDecl*, const SourceManager&);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::string Tsepepe::fully_expand_method_declaration(const CXXMethodDecl* method, const SourceManager& source_manager)
{
    std::string result;
    result.reserve(120);

    LangOptions lang_opts;
    PrintingPolicy printing_policy{lang_opts};
    printing_policy.adjustForCPlusPlus();
    printing_policy.FullyQualifiedName = true;
    printing_policy.Indentation = 4;
    printing_policy.CleanUglifiedParameters = true;
    printing_policy.AlwaysIncludeTypeForTemplateArgument = true;
    printing_policy.TerseOutput = true;
    printing_policy.PolishForDeclaration = true;
    printing_policy.PrintInjectedClassNameWithArguments = true;

    auto return_type_as_string{get_return_type(method, source_manager, printing_policy)};
    if (not return_type_as_string.empty())
    {
        result.append(std::move(return_type_as_string));
        result += ' ';
    }

    result.append(method->getQualifiedNameAsString());
    result.append(get_parameters(method, source_manager));

    return result;
}

// --------------------------------------------------------------------------------------------------------------------
// Private definitions
// --------------------------------------------------------------------------------------------------------------------
static std::string
get_return_type(const CXXMethodDecl* node, const SourceManager& source_manager, const PrintingPolicy& printing_policy)
{
    auto has_explicit_return_type{[&](const CXXMethodDecl* node) {
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
    std::string result;
    result.reserve(100);

    llvm::raw_string_ostream os{result};

    auto template_name{template_spec_type->getTemplateName()};
    template_name.print(os, printing_policy, TemplateName::Qualified::Fully);

    os << '<';
    const auto& template_args{template_spec_type->template_arguments()};
    for (const auto& template_arg : template_args)
    {
        template_arg.print(printing_policy, os, true);
        os << ", ";
    }

    if (template_args.size() > 0)
        // Replace the trailing ", " .
        result.erase(result.size() - 2);

    os << '>';

    return result;
}

static std::string get_parameters(const CXXMethodDecl* node, const SourceManager& source_manager)
{
    std::string result;
    result.reserve(20);

    result += '(';

    // if (auto param_source_range{node->getParametersSourceRange()}; param_source_range.isValid())
    // {
    //     auto parameters_as_is{source_range_content_to_string(param_source_range, source_manager)};
    //     std::regex default_param_regex{"\\s*=\\s*.*?(,|$)"};
    //     output_stream << std::regex_replace(parameters_as_is, default_param_regex, "$1");
    // }
    //
    result += ')';

    return result;
}

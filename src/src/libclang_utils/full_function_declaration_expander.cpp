/**
 * @file	full_function_declaration_expander.cpp
 * @brief	Implements the full expanding of the the function declaration.
 */
#include <algorithm>
#include <iterator>
#include <numeric>

#include "libclang_utils/full_function_declaration_expander.hpp"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>

#include "scope_remover.hpp"

#include "libclang_utils/misc_utils.hpp"
#include "string_utils.hpp"

using namespace clang;

// --------------------------------------------------------------------------------------------------------------------
// Private declarations
// --------------------------------------------------------------------------------------------------------------------

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
        return Tsepepe::utils::join(begin, end, " ");
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

        return Tsepepe::utils::join(standard_attributes_as_strings, " ");
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

        return type_to_string(node->getReturnType(), node->getASTContext());
    }

    std::string stringify_template_specialization(const TemplateSpecializationType* template_spec_type) const
    {
        const auto& ast_context{template_spec_type->getAsRecordDecl()->getASTContext()};
        auto template_arg_to_string{[&](const TemplateArgument& template_arg) {
            return type_to_string(template_arg.getAsType(), ast_context);
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

        os << '<' << Tsepepe::utils::join(template_args_as_string) << '>';

        return result;
    }

    std::string get_parameters(const FunctionDecl* node) const
    {
        auto result{get_parameters_with_fully_qualified_types(node)};

        if (options.remove_scope_from_parameters)
        {
            Tsepepe::FullyQualifiedName scope{node->getQualifiedNameAsString()};
            result = Tsepepe::AllScopeRemover{std::move(scope)}.remove_from(result);
        }

        return result;
    }

    std::string get_parameters_with_fully_qualified_types(const FunctionDecl* node) const
    {
        auto param_to_string{[&](const ParmVarDecl* param) {
            auto result{type_to_string(param->getType(), node->getASTContext())};
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

        return '(' + Tsepepe::utils::join(params_as_string) + ')';
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

    std::string type_to_string(QualType qual_type, const ASTContext& ast_context) const
    {
        auto is_elaborated_type{[&](const QualType& qual_type) {
            return qual_type.getTypePtr()->getAs<ElaboratedType>() != nullptr;
        }};

        if (is_elaborated_type(qual_type))
            qual_type = qual_type.getSingleStepDesugaredType(ast_context);

        auto type{qual_type.getTypePtr()};

        // TypedefName type are, according to clang AST dump, not only the 'typedef' created types, but also aliases
        // created with using. E.g. std::string is such a TypedefName type, and we don't want to desugarize it down
        // to std::basic_string<char>. We don't want to desugarize type aliases as well, therefore we print them as
        // they are.
        if (not type->isTypedefNameType())
            if (auto template_spec_type{type->getAs<TemplateSpecializationType>()}; template_spec_type != nullptr)
                return stringify_template_specialization(template_spec_type);

        return qual_type.getAsString(printing_policy);
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

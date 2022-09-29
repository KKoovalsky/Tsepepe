/**
 * @file	definition_generator.cpp
 * @brief	Implements the MethodDefinitionGenerator.
 */

#include <experimental/iterator>
#include <iostream>

#include "definition_generator.hpp"

using namespace clang;

namespace CppTinyRefactor
{

DefinitionGenerator::DefinitionGenerator(FileWithDeclaration f, LineWithDeclaration l) :
    file{std::move(f.value)}, line_number{l.value}, printing_policy(LangOptions())
{
    printing_policy.SuppressTagKeyword = 1;
}

void DefinitionGenerator::run(const clang::ast_matchers::MatchFinder::MatchResult& result)
{
    auto node{result.Nodes.getNodeAs<clang::CXXMethodDecl>("method declaration")};
    if (node == nullptr)
        return;

    auto& source_manager{result.Context->getSourceManager()};
    auto source_location{node->getLocation()};
    auto presumed_loc{source_manager.getPresumedLoc(source_location)};

    auto matched_node_file_name{presumed_loc.getFilename()};
    auto matched_node_line_nr{presumed_loc.getLine()};

    if (matched_node_line_nr != line_number or matched_node_file_name != file)
        return;

    if (node->isImplicit())
        return;

    print_return_type_if_any(node);
    print_name(node);
    print_parameters(node);
    print_const_qualifier_if_has_one(node);

    std::cout << std::endl;
}

void DefinitionGenerator::print_return_type_if_any(const clang::CXXMethodDecl* node) const
{
    if (auto kind{node->getKind()}; kind != Decl::Kind::CXXConstructor and kind != Decl::Kind::CXXDestructor)
        std::cout << node->getReturnType().getAsString(printing_policy) << ' ';
}

void DefinitionGenerator::print_name(const clang::CXXMethodDecl* node) const
{
    std::cout << node->getQualifiedNameAsString();
}

void DefinitionGenerator::print_parameters(const clang::CXXMethodDecl* node) const
{
    std::vector<std::string> params_stringified;
    params_stringified.reserve(node->parameters().size());

    for (auto param : node->parameters())
    {
        auto type{param->getType().getAsString(printing_policy)};
        auto name{param->getNameAsString()};

        std::string param_as_string{type + ' ' + name};
        params_stringified.emplace_back(std::move(param_as_string));
    }

    std::cout << '(';
    std::copy(std::begin(params_stringified),
              std::end(params_stringified),
              std::experimental::make_ostream_joiner(std::cout, ", "));
    std::cout << ')';
}

void DefinitionGenerator::print_const_qualifier_if_has_one(const clang::CXXMethodDecl* node) const
{
    // if (auto method_node{dynamic_cast<const clang::CXXMethodDecl*>(node)}; method_node != nullptr)
    //     if (method_node->isConst())
    //         std::cout << " const";
    if (node->isConst())
        std::cout << " const";
}

} // namespace CppTinyRefactor

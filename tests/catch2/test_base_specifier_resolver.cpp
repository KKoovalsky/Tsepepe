/**
 * @file        test_base_specifier_resolver.cpp
 * @brief       Tests the base specifier resolver.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

#include <string>

#include "common_types.hpp"
#include "libclang_utils/base_specifier_resolver.hpp"

#include "clang_ast_fixtures.hpp"

namespace BaseSpecifierResolverTest
{
struct TestCase
{
    std::string description;
    std::string header_file_content;
    std::string deriving_class_name;
    std::string base_class_name;
    Tsepepe::CodeInsertion expected_result;
};
}; // namespace BaseSpecifierResolverTest

TEST_CASE("Base specifier is resolved", "[BaseSpecifierResolver]")
{
    using namespace clang;
    using namespace BaseSpecifierResolverTest;

    auto [description, header_file_content, deriving_class_name, base_class_name, expected_result] = GENERATE(values({
        TestCase{.description = "For a struct which does not inherit, creates a base-clause",
                 .header_file_content = "struct Base\n"
                                        "{\n"
                                        "};\n"
                                        "\n"
                                        "struct Derived\n"
                                        "{\n"
                                        "};\n",
                 .deriving_class_name = "Derived",
                 .base_class_name = "Base",
                 .expected_result =
                     {
                         .code = " : Base",
                         .line = 5,
                         .column = 15,
                     }},
        TestCase{.description =
                     "For a struct deriving from few types, the base-clause is extended, without 'public' keyword"},
        TestCase{.description = "For a struct with multi-line base-clause, it is extended without 'public' keyword"},
        TestCase{.description = "For a class which does not inherit, creats a base-clause"},
        TestCase{.description =
                     "For a class deriving from few types, the base-clause is extended, with the 'public' keyword"},
        TestCase{.description = "For a clas with multi-line base-clause, it is extended with the 'public' keyword"},
        TestCase{.description = "Skips extending the base-clause, if the base-specifier is already in place"},
        TestCase{.description = "Creates base-clause before the class body opening bracket, if it is inline"},

    }));

    INFO(description);

    // FIXME: Remove me!
    REQUIRE(not header_file_content.empty());

    Tsepepe::ClangSingleAstFixture ast_fixture{header_file_content};

    auto make_class_matcher_by_name{[](const std::string& name) {
        return ast_matchers::cxxRecordDecl(ast_matchers::hasName(name)).bind("class");
    }};

    auto deriving_class_matcher{make_class_matcher_by_name(deriving_class_name)};
    auto base_class_matcher{make_class_matcher_by_name(base_class_name)};

    auto deriving_class{ast_fixture.get_first_match<CXXRecordDecl>(deriving_class_matcher)};
    auto base_class{ast_fixture.get_first_match<CXXRecordDecl>(base_class_matcher)};

    auto result{Tsepepe::resolve_base_specifier(header_file_content, deriving_class, base_class)};
    REQUIRE(result == expected_result);
}

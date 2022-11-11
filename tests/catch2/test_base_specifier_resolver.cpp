/**
 * @file        test_base_specifier_resolver.cpp
 * @brief       Tests the base specifier resolver.
 */
#include "ostream_printers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

#include <iostream>
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

    auto [description, header_file_content, deriving_class_name, base_class_name, expected_result] =
        GENERATE(values({
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
                         "For a struct deriving from few types, the base-clause is extended without 'public' keyword",
                     .header_file_content = "struct Base1{};\n"
                                            "struct Base2{};\n"
                                            "struct Derived : Base1\n"
                                            "{};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base2",
                     .expected_result = {.code = ", Base2", .line = 3, .column = 23}},
            TestCase{.description = "For a struct with multi-line base-clause, it is extended without 'public' keyword",
                     .header_file_content = "struct Base1{};\n"
                                            "struct Base2{};\n"
                                            "struct Base3{};\n"
                                            "struct Derived : Base1,\n"
                                            "                 Base2\n"
                                            "{};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base3",
                     .expected_result = {.code = ", Base3", .line = 5, .column = 23}},
            TestCase{.description = "For a class which does not inherit, creates a base-clause",
                     .header_file_content = "struct Base\n"
                                            "{\n"
                                            "};\n"
                                            "\n"
                                            "class Derived\n"
                                            "{\n"
                                            "};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base",
                     .expected_result =
                         {
                             .code = " : public Base",
                             .line = 5,
                             .column = 14,
                         }},
            TestCase{.description =
                         "For a class deriving from few types, the base-clause is extended, with the 'public' keyword",
                     .header_file_content = "struct Base1{};\n"
                                            "struct Base2{};\n"
                                            "struct Base3{};\n"
                                            "class Derived : public Base1, public Base3\n"
                                            "{};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base2",
                     .expected_result = {.code = ", public Base2", .line = 4, .column = 43}},
            TestCase{.description = "For a class with multi-line base-clause, it is extended with the 'public' keyword",
                     .header_file_content = "struct Maka{};\n"
                                            "struct Paka{};\n"
                                            "struct Masta{};\n"
                                            "struct Pasta{};\n"
                                            "class Derived : public Maka,\n"
                                            "                public Masta,\n"
                                            "                public Pasta\n"
                                            "{};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Paka",
                     .expected_result = {.code = ", public Paka", .line = 7, .column = 29}},
            TestCase{.description = "Skips extending the base-clause, if the base-specifier is already in place",
                     .header_file_content = "struct Maka{};\n"
                                            "struct Masta{};\n"
                                            "struct Pasta{};\n"
                                            "class Derived : public Maka,\n"
                                            "                public Masta,\n"
                                            "                public Pasta\n"
                                            "{};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Masta",
                     .expected_result = {.code = "", .line = 0, .column = 0}},
            TestCase{.description =
                         "Creates base-clause before the class body opening bracket, if the bracket is inline",
                     .header_file_content = "struct Base {};\n"
                                            "\n"
                                            "class Derived {\n"
                                            "};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base",
                     .expected_result =
                         {
                             .code = " : public Base",
                             .line = 3,
                             .column = 14,
                         }},
            TestCase{.description =
                         "Creates base-clause before the class body opening bracket, when there is a comment "
                         "between the class declaration line and the opening bracket",
                     .header_file_content = "struct Base {};\n"
                                            "struct Derived // Yolo comment \n"
                                            "{\n"
                                            "};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base",
                     .expected_result =
                         {
                             .code = " : Base",
                             .line = 2,
                             .column = 15,
                         }},
            TestCase{.description =
                         "Appends a base-specifier before the class body opening bracket, when there is a comment "
                         "between the base-clause and the opening bracket",
                     .header_file_content = "struct Base1 {};\n"
                                            "struct Base2 {};\n"
                                            "struct Base3 {};\n"
                                            "struct Derived : Base1, Base3 // Yolo comment \n"
                                            "{\n"
                                            "};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base2",
                     .expected_result =
                         {
                             .code = ", Base2",
                             .line = 4,
                             .column = 30,
                         }},
            TestCase{.description = "Fully qualified base class name is appended, if no scope is shared",
                     .header_file_content = "namespace Yolo {\n"
                                            "struct Base {};\n"
                                            "}\n"
                                            "struct Derived\n"
                                            "{\n"
                                            "};\n",
                     .deriving_class_name = "Derived",
                     .base_class_name = "Base",
                     .expected_result =
                         {
                             .code = " : Yolo::Base",
                             .line = 4,
                             .column = 15,
                         }},
            TestCase{
                .description =
                    "Namespace name is shortified, if the base class and the derived class are in the same namespace",
                .header_file_content = "namespace Yolo {\n"
                                       "struct Base {};\n"
                                       "struct Derived\n"
                                       "{\n"
                                       "};\n"
                                       "}\n",
                .deriving_class_name = "Derived",
                .base_class_name = "Base",
                .expected_result =
                    {
                        .code = " : Base",
                        .line = 3,
                        .column = 15,
                    }},
        }));

    INFO(description);

    Tsepepe::ClangSingleAstFixture ast_fixture{header_file_content};

    auto make_class_matcher_by_name{[](const std::string& name) {
        return ast_matchers::cxxRecordDecl(ast_matchers::hasName(name)).bind("class");
    }};

    auto deriving_class_matcher{make_class_matcher_by_name(deriving_class_name)};
    auto base_class_matcher{make_class_matcher_by_name(base_class_name)};

    auto deriving_class{ast_fixture.get_first_match<CXXRecordDecl>(deriving_class_matcher)};
    auto base_class{ast_fixture.get_first_match<CXXRecordDecl>(base_class_matcher)};

    auto result{Tsepepe::resolve_base_specifier(
        header_file_content, deriving_class, base_class, ast_fixture.get_source_manager())};
    REQUIRE(result == expected_result);
}

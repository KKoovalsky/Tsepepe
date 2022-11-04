/**
 * @file        test_full_method_declaration_expander.cpp
 * @brief       Tests the full expander of the method declaration.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <initializer_list>
#include <string>

#include "libclang_utils/full_method_declaration_expander.hpp"

#include "clang_ast_fixtures.hpp"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

namespace FullMethodDeclarationExpanderTest
{
struct SingleHeaderFileTestData
{
    std::string description;
    std::string header_file_content;
    unsigned line_with_declaration;
    std::string expected_result;
};
}; // namespace FullMethodDeclarationExpanderTest

AST_MATCHER_P(clang::CXXMethodDecl, isDeclaredAtLine, unsigned, line)
{
    const auto& source_manager{Finder->getASTContext().getSourceManager()};
    auto actual_line_with_declaration{source_manager.getPresumedLoc(Node.getBeginLoc()).getLine()};
    return line == actual_line_with_declaration;
};

TEST_CASE("Method declarations are expanded fully", "[FullMethodDeclarationExpander]")
{
    using namespace Tsepepe;
    using namespace FullMethodDeclarationExpanderTest;

    SECTION("For a single header file")
    {
        static std::initializer_list<SingleHeaderFileTestData> test_data{
            SingleHeaderFileTestData{.description = "From basic method declaration",
                                     .header_file_content = "class SomeClass\n"
                                                            "{\n"
                                                            "    unsigned int foo();\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "unsigned int SomeClass::foo()\n"},
            SingleHeaderFileTestData{.description = "From declaration returning a nested type",
                                     .header_file_content = "class Foo\n"
                                                            "{\n"
                                                            "    class Bar\n"
                                                            "    {\n"
                                                            "    };\n"
                                                            "\n"
                                                            "    Bar yolo();\n"
                                                            "};\n",
                                     .line_with_declaration = 7,
                                     .expected_result = "Foo::Bar Foo::yolo()\n"},
            SingleHeaderFileTestData{.description = "From method returning multi-nested type",
                                     .header_file_content = "class Class\n"
                                                            "{\n"
                                                            "    class Nested\n"
                                                            "    {\n"
                                                            "        struct EvenMoreNested\n"
                                                            "        {\n"
                                                            "            EvenMoreNested create();\n"
                                                            "        };\n"
                                                            "    };\n"
                                                            "};\n",
                                     .line_with_declaration = 7,
                                     .expected_result =
                                         "Class::Nested::EvenMoreNested Class::Nested::EvenMoreNested::create()\n"}};

        auto [description, header_file_content, line_with_declaration, expected_result] = GENERATE(values(test_data));

        INFO(description);

        using namespace clang;
        auto matcher{
            ast_matchers::cxxMethodDecl(isDeclaredAtLine(line_with_declaration)).bind("method_at_line_matcher")};

        ClangSingleAstFixture ast_fixture{header_file_content};
        auto method{ast_fixture.get_first_match<CXXMethodDecl>(matcher)};

        REQUIRE(fully_expand_method_declaration(method) == expected_result);
    }
}

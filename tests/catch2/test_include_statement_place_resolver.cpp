/**
 * @file        test_include_statement_place_resolver.cpp
 * @brief       Tests the include resolver.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "include_statement_place_resolver.hpp"

#include "clang_ast_fixtures.hpp"

using namespace Tsepepe;

namespace IncludeStatementPlaceResolverTest
{
struct TestData
{
    std::string description;
    std::string header_file_content;
    IncludeStatementPlace expected_result;
};
}; // namespace IncludeStatementPlaceResolverTest

TEST_CASE("A new include place is resolved", "[IncludeStatementPlaceResolver]")
{

    using namespace IncludeStatementPlaceResolverTest;
    auto [description, header_file_content, expected_result] = GENERATE(values({
        TestData{.description = "Resolves to the top of the file if no include is found",
                 .header_file_content = "",
                 .expected_result = {.line = 1, .is_newline_needed = false}},
        TestData{.description = "Resolves the top of the file, just below the comment header, if no include is found",
                 .header_file_content = "/**\n"
                                        " * @file	test_include_resolver.hpp\n"
                                        " * @brief	Yolo description.\n"
                                        " */\n",
                 .expected_result = {.line = 4, .is_newline_needed = true}},
        TestData{.description = "Resolves just below a larger comment header, if no include is found",
                 .header_file_content = "/**\n"
                                        " * @file	test_include_resolver.hpp\n"
                                        " * @brief	Yolo description.\n"
                                        " *\n"
                                        " * This is some more description.\n"
                                        " * I guess it should contain lorem ipsum ...\n"
                                        " * Bang bang! Yolo!\n"
                                        " */ \n"
                                        "struct SomeStruct {\n"
                                        "\n"
                                        "};",
                 .expected_result = {.line = 8, .is_newline_needed = true}},
        TestData{.description = "Resolves within the include guards, if no include statement is found",
                 .header_file_content = "#ifndef TEST_INCLUDE_STATEMENT_PLACE_RESOLVER_CPP\n"
                                        "#define TEST_INCLUDE_STATEMENT_PLACE_RESOLVER_CPP\n"
                                        "\n"
                                        "\n"
                                        "\n"
                                        "#endif /* TEST_INCLUDE_STATEMENT_PLACE_RESOLVER_CPP */\n",
                 .expected_result = {.line = 2, .is_newline_needed = true}},
        TestData{.description =
                     "Resolves within the include guards, below comment header, if no include statement is found",
                 .header_file_content = "/**\n"
                                        " * @file	test_include_statement_place_resolver.cpp\n"
                                        " * @brief	Some yolo description.\n"
                                        " */\n"
                                        "#ifndef TASTA\n"
                                        "#define TASTA\n"
                                        "\n"
                                        "\n"
                                        "\n"
                                        "#endif /* TASTA */\n",
                 .expected_result = {.line = 6, .is_newline_needed = true}},
        TestData{.description = "Resolves to the line with 'pragma once' statement, if no include statement is found",
                 .header_file_content = "/**\n"
                                        " * @file	test_include_statement_place_resolver.cpp\n"
                                        " * @brief	Some yolo description.\n"
                                        " */\n"
                                        "#pragma once\n"
                                        "\n"
                                        "struct Struct {};\n",
                 .expected_result = {.line = 5, .is_newline_needed = true}},
        TestData{.description = "Resolves to the line with the last \"...\" include, for a bare file",
                 .header_file_content = "\n"
                                        "\n"
                                        "#include \"temp/yolo.hpp\"\n"
                                        "#include \"temp/dummy.hpp\"\n"
                                        "\n"
                                        "#include \"temp/dang.hpp\"\n"
                                        "\n",
                 .expected_result = {.line = 6, .is_newline_needed = false}},
        TestData{.description =
                     "Resolves to the line with the last \"...\" include, for a 'proper' header file content",
                 .header_file_content = "/**\n"
                                        " * @file        test_include_statement_place_resolver.cpp\n"
                                        " * @brief       Yolo descr.\n"
                                        " */\n"
                                        "#ifndef MASTAA\n"
                                        "#define MASTAA\n"
                                        "\n"
                                        "#include \"temp/gimme/this.hpp\"\n"
                                        "#include \"temp/gime/that.hpp\"\n"
                                        "\n"
                                        "class Gimme\n"
                                        "{\n"
                                        "};\n"
                                        "\n"
                                        "#endif /* MASTAA */\n",
                 .expected_result = {.line = 9, .is_newline_needed = false}},
        TestData{
            .description =
                "Resolves to the line with the last <...> include, if no \"...\" include is found, for a bare file",
            .header_file_content = "#include <string>\n"
                                   "#include <vector>\n"
                                   "\n"
                                   "#include <utility>\n"
                                   "\n"
                                   "\n",
            .expected_result = {.line = 4, .is_newline_needed = true}},
        TestData{.description = "Resolves to the line with the last <...> include, for a 'proper' header file content",
                 .header_file_content = "/**\n"
                                        " * @file        test_include_statement_place_resolver.cpp\n"
                                        " * @brief       Yolo descr.\n"
                                        " */\n"
                                        "#ifndef MASTAA\n"
                                        "#define MASTAA\n"
                                        "\n"
                                        "#include <string>\n"
                                        "#include <vector>\n"
                                        "\n"
                                        "class Gimme\n"
                                        "{\n"
                                        "};\n"
                                        "\n"
                                        "#endif /* MASTAA */\n",
                 .expected_result = {.line = 9, .is_newline_needed = true}},
        TestData{.description = "Resolves to the line with the last \"...\", when both include types exist",
                 .header_file_content = "/**\n"
                                        " * @file        test_include_statement_place_resolver.cpp\n"
                                        " * @brief       Yolo descr.\n"
                                        " */\n"
                                        "#ifndef MASTAA\n"
                                        "#define MASTAA\n"
                                        "\n"
                                        "#include \"temp/yolo.hpp\"\n"
                                        "#include \"temp/dummy.hpp\"\n"
                                        "\n"
                                        "#include <string>\n"
                                        "#include <vector>\n"
                                        "\n"
                                        "#include \"temp/gimme/this.hpp\"\n"
                                        "#include \"temp/gime/that.hpp\"\n"
                                        "\n"
                                        "class Gimme\n"
                                        "{\n"
                                        "};\n"
                                        "\n"
                                        "#endif /* MASTAA */\n",
                 .expected_result = {.line = 15, .is_newline_needed = false}},
    }));

    INFO(description);
    auto result{resolve_include_statement_place(header_file_content)};
    REQUIRE(result == expected_result);
}

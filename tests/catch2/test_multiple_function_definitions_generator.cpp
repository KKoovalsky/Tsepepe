/**
 * @file        test_multiple_function_definitions_generator.cpp
 * @brief       Tests multiple functions definitions generation.
 */
#include <filesystem>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <clang/Tooling/CompilationDatabase.h>

#include "base_error.hpp"
#include "generate_function_definitions_code_action.hpp"

namespace MultipleFunctionDefinitionsGeneratorTest
{
struct TestData
{
    std::string description;
    std::string header_file_content;
    unsigned selected_line_begin;
    unsigned selected_line_end;
    std::string expected_result;
};
}; // namespace MultipleFunctionDefinitionsGeneratorTest

TEST_CASE("Multiple function definitions are generated at once", "[FunctionDefinitionGenerator]")
{
    using namespace MultipleFunctionDefinitionsGeneratorTest;
    using namespace Tsepepe;
    namespace fs = std::filesystem;

    std::string err;
    static std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database{
        clang::tooling::CompilationDatabase::loadFromDirectory(COMPILATION_DATABASE_DIR, err)};
    if (compilation_database == nullptr)
        throw std::runtime_error{"Failed to load the compilation database: " + err};

    SECTION("Positive tests")
    {
        static std::initializer_list<TestData> test_data{
            TestData{.description = "From few methods declarations",
                     .header_file_content = "/**\n"
                                            " * @file        test_multiple_function_definitions_generator.cpp\n"
                                            " * @brief\n"
                                            " */\n"
                                            "#ifndef TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "#define TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "\n"
                                            "#include <string>\n"
                                            "#include <vector>\n"
                                            "\n"
                                            "namespace detail\n"
                                            "{\n"
                                            "    struct Baz{};\n"
                                            "}\n"
                                            "\n"
                                            "class Boo\n"
                                            "{\n"
                                            "  public:\n"
                                            "    explicit Boo();\n"
                                            "\n"
                                            "    detail::Baz do_stuff() const;\n"
                                            "    std::vector<detail::Baz> gimme(std::string name);\n"
                                            "\n"
                                            "  private:\n"
                                            "};\n"
                                            "\n"
                                            "#endif /* TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP */\n",
                     .selected_line_begin = 19,
                     .selected_line_end = 22,
                     .expected_result = "Boo::Boo()\n"
                                        "{\n"
                                        "}\n"
                                        "\n"
                                        "detail::Baz Boo::do_stuff() const\n"
                                        "{\n"
                                        "}\n"
                                        "\n"
                                        "std::vector<detail::Baz> Boo::gimme(std::string name)\n"
                                        "{\n"
                                        "}\n"},
            TestData{.description = "Ignores any other entities that are not function declarations",
                     .header_file_content = "/**\n"
                                            " * @file        test_multiple_function_definitions_generator.cpp\n"
                                            " * @brief\n"
                                            " */\n"
                                            "#ifndef TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "#define TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "\n"
                                            "#include <string>\n"
                                            "#include <vector>\n"
                                            "\n"
                                            "namespace detail\n"
                                            "{\n"
                                            "    struct Baz{};\n"
                                            "}\n"
                                            "\n"
                                            "class Boo\n"
                                            "{\n"
                                            "  public:\n"
                                            "    explicit Boo();\n"
                                            "\n"
                                            "  private:\n"
                                            "    struct Basta {};\n"
                                            "    detail::Baz do_stuff() const;\n"
                                            "    std::vector<detail::Baz> gimme(std::string name);\n"
                                            "\n"
                                            "};\n"
                                            "\n"
                                            "#endif /* TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP */\n",
                     .selected_line_begin = 17,
                     .selected_line_end = 25,
                     .expected_result = "Boo::Boo()\n"
                                        "{\n"
                                        "}\n"
                                        "\n"
                                        "detail::Baz Boo::do_stuff() const\n"
                                        "{\n"
                                        "}\n"
                                        "\n"
                                        "std::vector<detail::Baz> Boo::gimme(std::string name)\n"
                                        "{\n"
                                        "}\n"},
            TestData{.description = "Ignores function definitions",
                     .header_file_content = "/**\n"
                                            " * @file        test_multiple_function_definitions_generator.cpp\n"
                                            " * @brief\n"
                                            " */\n"
                                            "#ifndef TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "#define TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "\n"
                                            "#include <string>\n"
                                            "#include <vector>\n"
                                            "\n"
                                            "namespace detail\n"
                                            "{\n"
                                            "    struct Baz{};\n"
                                            "}\n"
                                            "\n"
                                            "class Boo\n"
                                            "{\n"
                                            "  public:\n"
                                            "    explicit Boo(std::string);\n"
                                            "    Boo() = default;\n"
                                            "\n"
                                            "  private:\n"
                                            "    struct Basta {};\n"
                                            "    detail::Baz do_stuff() const { return {}; } \n"
                                            "    void foo() {}\n"
                                            "    std::vector<detail::Baz> gimme(std::string name);\n"
                                            "\n"
                                            "};\n"
                                            "\n"
                                            "#endif /* TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP */\n",
                     .selected_line_begin = 17,
                     .selected_line_end = 27,
                     .expected_result = "Boo::Boo(std::string)\n"
                                        "{\n"
                                        "}\n"
                                        "\n"
                                        "std::vector<detail::Baz> Boo::gimme(std::string name)\n"
                                        "{\n"
                                        "}\n"},
            TestData{.description = "Implements multiple free functions",
                     .header_file_content = "/**\n"
                                            " * @file        test_multiple_function_definitions_generator.cpp\n"
                                            " * @brief\n"
                                            " */\n"
                                            "#ifndef TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "#define TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "\n"
                                            "#include <string>\n"
                                            "#include <vector>\n"
                                            "#include <utility>\n"
                                            "\n"
                                            "struct Bar{};\n"
                                            "\n"
                                            "namespace Basta\n"
                                            "{\n"
                                            "\n"
                                            "struct Baz{};\n"
                                            "\n"
                                            "std::pair<Bar, Baz> gimme(std::vector<std::pair<std::string, Baz>>);\n"
                                            "inline void foo() {}\n"
                                            "std::string to_string(std::vector<Baz>);\n"
                                            "}\n"
                                            "#endif /* TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP */\n",
                     .selected_line_begin = 14,
                     .selected_line_end = 22,
                     .expected_result =
                         "std::pair<Bar, Basta::Baz> Basta::gimme(std::vector<std::pair<std::string, Basta::Baz>>)\n"
                         "{\n"
                         "}\n"
                         "\n"
                         "std::string Basta::to_string(std::vector<Basta::Baz>)\n"
                         "{\n"
                         "}\n"},
            TestData{.description = "Prints out an empty string if no declaration found within the selected range",
                     .header_file_content = "/**\n"
                                            " * @file        test_multiple_function_definitions_generator.cpp\n"
                                            " * @brief\n"
                                            " */\n"
                                            "#ifndef TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "#define TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "\n"
                                            "#include <string>\n"
                                            "#include <vector>\n"
                                            "#include <utility>\n"
                                            "\n"
                                            "struct Bar{};\n"
                                            "\n"
                                            "namespace Basta\n"
                                            "{\n"
                                            "\n"
                                            "struct Baz{};\n"
                                            "\n"
                                            "std::pair<Bar, Baz> gimme(std::vector<std::pair<std::string, Baz>>);\n"
                                            "inline void foo() {}\n"
                                            "std::string to_string(std::vector<Baz>);\n"
                                            "}\n"
                                            "#endif /* TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP */\n",
                     .selected_line_begin = 13,
                     .selected_line_end = 14,
                     .expected_result = ""},
        };

        auto [description, header_file_content, selected_line_begin, selected_line_end, expected_result] =
            GENERATE(values(test_data));

        auto result{GenerateFunctionDefinitionsCodeActionLibclangBased{compilation_database}.apply(
            {.source_file_path = fs::temp_directory_path(),
             .source_file_content = std::move(header_file_content),
             .selected_line_begin = selected_line_begin,
             .selected_line_end = selected_line_end})};

        REQUIRE(result == expected_result);
    }

    SECTION("Negative tests")
    {
        SECTION("Throws if selected line begin is after the end")
        {
            auto do_apply{[]() {
                GenerateFunctionDefinitionsCodeActionLibclangBased{compilation_database}.apply(
                    {.source_file_path = fs::temp_directory_path(),
                     .source_file_content = "/**\n"
                                            " * @file        test_multiple_function_definitions_generator.cpp\n"
                                            " * @brief\n"
                                            " */\n"
                                            "#ifndef TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "#define TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP\n"
                                            "\n"
                                            "#include <string>\n"
                                            "#include <vector>\n"
                                            "#include <utility>\n"
                                            "\n"
                                            "struct Bar{};\n"
                                            "\n"
                                            "namespace Basta\n"
                                            "{\n"
                                            "\n"
                                            "struct Baz{};\n"
                                            "\n"
                                            "std::pair<Bar, Baz> gimme(std::vector<std::pair<std::string, Baz>>);\n"
                                            "inline void foo() {}\n"
                                            "std::string to_string(std::vector<Baz>);\n"
                                            "}\n"
                                            "#endif /* TEST_MULTIPLE_FUNCTION_DEFINITIONS_GENERATOR_CPP */\n",
                     .selected_line_begin = 4,
                     .selected_line_end = 3});
            }};

            REQUIRE_THROWS_AS(do_apply(), Tsepepe::BaseError);
        }
    }
}

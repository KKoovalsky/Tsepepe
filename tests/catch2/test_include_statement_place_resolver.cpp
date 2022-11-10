/**
 * @file        test_include_statement_place_resolver.cpp
 * @brief       Tests the include resolver.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "libclang_utils/include_statement_place_resolver.hpp"

#include "clang_ast_fixtures.hpp"

using namespace Tsepepe;

namespace fs = std::filesystem;

namespace IncludeStatementPlaceResolverTest
{
struct TestData
{
    std::string description;
    fs::path path_to_include;
    std::string header_file_content;
    unsigned expected_result;
};
}; // namespace IncludeStatementPlaceResolverTest

TEST_CASE("A new include place is resolved", "[IncludeStatementPlaceResolver]")
{
    using namespace IncludeStatementPlaceResolverTest;
    auto [description, path_to_include, header_file_content, expected_result] = GENERATE(values({
        TestData{.description = "Resolves to the top of the file if no include is found",
                 .path_to_include = "/some/path/to/header.hpp",
                 .header_file_content = "",
                 .expected_result = 1},
        TestData{.description = "Resolves the top of the file, just below the comment header, if no include is found",
                 .path_to_include = "/path/to/yolo.hpp",
                 .header_file_content = "/**\n"
                                        " * @file	test_include_resolver.hpp\n"
                                        " * @brief	Yolo description.\n"
                                        " */\n",
                 .expected_result = 4},
    }));

    INFO(description);
    ClangSingleAstFixture fixture{header_file_content};
    auto result{resolve_include_statement_place(path_to_include, header_file_content, fixture.get_ast_unit())};
    REQUIRE(result == expected_result);

    // SECTION("Resolves to the top of the file if no include is found")
    // {
    //     // REQUIRE(resolve_local_include("/some/path/to/header.hpp", "") == 1);
    // }
    //
    // SECTION("Resolves the top of the file, just below the comment header, if no include is found")
    // {
    //     // REQUIRE(resolve_local_include("/path/to/yolo.hpp",
    //     //                               "/**\n"
    //     //                               " * @file	test_include_resolver.hpp\n"
    //     //                               " * @brief	Yolo description.\n"
    //     //                               " */\n")
    //     //         == 4);
    // }
    //
    // SECTION("Resolves to two lines below the last <...> include, if no \"...\" includes are found")
    // {
    // }
    //
    // SECTION("Resolves to one line below the last \"...\" include")
    // {
    //     // Various scenarios: multiple \"...\" sections, intervleaved with <...>, etc.
    // }
    //
    // SECTION("Resolves nothing if the include is already found")
    // {
    // }
}

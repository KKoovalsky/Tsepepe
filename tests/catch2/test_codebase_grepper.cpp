/**
 * @file	test_codebase_grepper.cpp
 * @brief	Tests grepping through the codebase.
 */
#include <filesystem>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <ostream>

#include <iostream>

#include "codebase_grepper.hpp"
#include "directory_tree.hpp"

using namespace Tsepepe;

std::ostream& operator<<(std::ostream& os, const GrepMatch& match)
{
    return os << "Path: " << match.path << ", line: " << match.line << ", column: " << match.column;
};

TEST_CASE("Finds strings within a project", "[CodebaseGrepper]")
{
    GIVEN("Some dummy test files, where no matches shall be found")
    {
        DirectoryTree dir_tree("temp");
        dir_tree.create_file("yolo.txt", "some\ncontent\nbum\n");
        dir_tree.create_file("nested/bang.cpp", "struct DummySymbol {};");
        dir_tree.create_file("even/more/nested/basta.hpp", "struct SymbolDummy {};");
        dir_tree.create_file("dir1/bum.hpp", "class SomeSymbolsss {};\nvoid\nbum\nSymbolz");
        dir_tree.create_file("dir1/bo.hpp", "corelate\nme you fool!\nin the basement\non the desert\n");

        auto current_path{std::filesystem::current_path()};

        RootDirectory root_dir{"temp"};
        EcmaScriptPattern pattern{"\\b(struct|class)\\s+Symbol\\b"};

        THEN("No match is found")
        {
            REQUIRE(codebase_grep(root_dir, pattern).empty());
        }

        AND_GIVEN("File with a symbol somewhere in the middle")
        {
            dir_tree.create_file("dir2/piste.cpp",
                                 "some content\nyolo\nbejbi\nand the struct Symbol yolo\n and no\n more matches\n");

            THEN("A match is found")
            {
                auto r{codebase_grep(root_dir, pattern)};
                REQUIRE_THAT(codebase_grep(root_dir, pattern),
                             Catch::Matchers::Equals(std::vector<GrepMatch>{
                                 {.path = current_path / "temp/dir2/piste.cpp", .line = 4, .column = 9}}));
            }

            AND_GIVEN("More files with the same symbol")
            {
                dir_tree.create_file(
                    "dirs/dir3/casta.cpp",
                    "some content\nyolo\nbejss\nand the stuct Symbol yolo\n and \nclass Symbol\n more do matches\n");

                THEN("A match is found")
                {
                    REQUIRE_THAT(codebase_grep(root_dir, pattern),
                                 Catch::Matchers::Equals(std::vector<GrepMatch>{
                                     {.path = current_path / "temp/dir2/piste.cpp", .line = 4, .column = 9},
                                     {.path = current_path / "temp/dirs/dir3/casta.cpp", .line = 6, .column = 1}}));
                }
            }
        }
    }
}

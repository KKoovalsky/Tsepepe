/**
 * @file        test_symbol_finder.cpp
 * @brief       Tests finding symbols within a project.
 */
#include <filesystem>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "directory_tree.hpp"
#include "symbol_finder.hpp"

using namespace Tsepepe;

TEST_CASE("Finds symbols within a project.", "[SymbolFinder]")
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

        THEN("No match is found")
        {
            REQUIRE(find_symbol("temp", SymbolName{"Symbol"}).empty());
        }

        AND_GIVEN("File with a symbol somewhere in the middle")
        {
            dir_tree.create_file("dir2/piste.cpp",
                                 "some content\nyolo\nbejbi\nand the Symbol yolo\n and no\n more matches\n");

            THEN("A match is found")
            {
                REQUIRE_THAT(
                    find_symbol("temp", SymbolName{"Symbol"}),
                    Catch::Matchers::Equals(std::vector<SymbolMatch>{{.path = current_path / "temp/dir2/piste.cpp",
                                                                      .line = 4,
                                                                      .column_begin = 9,
                                                                      .column_end = 15}}));
            }

            AND_GIVEN("More files with the same symbol")
            {
                WHEN("Symbols is searched")
                {
                    THEN("A match is found")
                    {
                    }
                }
            }
        }
    }
}

/**
 * @file        test_code_insertions_applier.cpp
 * @brief       Tests applying of the code insertions.
 */
#include <catch2/catch_test_macros.hpp>

#include "code_insertions_applier.hpp"

using namespace Tsepepe;

TEST_CASE("Code insertions are applied", "[CodeInsertionApplier]")
{
    SECTION("Applies single code insertion")
    {
        std::string input{"yolo  bang!"};
        CodeInsertionByOffset insertion{.code = "makapaka", .offset = 5};
        REQUIRE(apply_insertions(input, {insertion}) == "yolo makapaka bang!");
    }

    SECTION("Applies multiple code insertions")
    {
        std::string input{"yolo  bang!  bum!"};
        CodeInsertionByOffset insertion{.code = "makapaka", .offset = 5};
        REQUIRE(apply_insertions(input, {{.code = "basta?", .offset = 5}, {.code = "szasta :)", .offset = 12}})
                == "yolo basta? bang! szasta :) bum!");
    }

    SECTION("Skips empty code insertions")
    {
    }

    SECTION("Raises error if one code insertion is out of bound")
    {
    }

    SECTION("Handles code insertions, which put stuff in the same place")
    {
    }

    SECTION("Applies code insertion exactly at the end")
    {
    }
}

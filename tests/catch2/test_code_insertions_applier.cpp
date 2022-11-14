/**
 * @file        test_code_insertions_applier.cpp
 * @brief       Tests applying of the code insertions.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include "base_error.hpp"
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

    SECTION("Applies unsorted code insertions")
    {
        std::string input{"SFINAE"};
        std::vector<CodeInsertionByOffset> insertions{{.code = "rror!", .offset = 6},
                                                      {.code = "n ", .offset = 5},
                                                      {.code = "ubstitution ", .offset = 1},
                                                      {.code = "s ", .offset = 3},
                                                      {.code = "ailure ", .offset = 2},
                                                      {.code = "ot ", .offset = 4}};
        REQUIRE(apply_insertions(input, std::move(insertions)) == "Substitution Failure Is Not An Error!");
    }

    SECTION("Skips empty code insertions")
    {
        std::string input{"SFINAE"};
        std::vector<CodeInsertionByOffset> insertions{{.code = "rror!", .offset = 6},
                                                      {.code = "n ", .offset = 5},
                                                      {.code = "", .offset = 4},
                                                      {.code = "ubstitution ", .offset = 1},
                                                      {.code = "s ", .offset = 3},
                                                      {.code = "", .offset = 1},
                                                      {.code = "ailure ", .offset = 2},
                                                      {.code = "", .offset = 2},
                                                      {.code = "ot ", .offset = 4}};
        REQUIRE(apply_insertions(input, std::move(insertions)) == "Substitution Failure Is Not An Error!");
    }

    SECTION("Handles code insertions, which put stuff in the same place")
    {
        std::string input{"Hello world!"};
        std::vector<CodeInsertionByOffset> insertions{{.code = ", I love you", .offset = 5},
                                                      {.code = ", do you love me", .offset = 5},
                                                      {.code = ", or does he,", .offset = 5}};
        REQUIRE(apply_insertions(input, std::move(insertions))
                == "Hello, I love you, do you love me, or does he, world!");
    }

    SECTION("Applies code insertion exactly at the end")
    {
        std::string input{"Hallochen"};
        std::vector<CodeInsertionByOffset> insertions{
            {.code = " my", .offset = 9}, {.code = " beautiful", .offset = 9}, {.code = " world!", .offset = 9}};
        REQUIRE(apply_insertions(input, std::move(insertions)) == "Hallochen my beautiful world!");
    }

    SECTION("Applies code insertion exactly at the beginning")
    {
        std::string input{"World!"};
        std::vector<CodeInsertionByOffset> insertions{
            {.code = "Hey, ", .offset = 0}, {.code = "you. ", .offset = 0}, {.code = "Dummy. ", .offset = 0}};
        REQUIRE(apply_insertions(input, std::move(insertions)) == "Hey, you. Dummy. World!");
    }

    SECTION("Raises error if one code insertion is out of bound")
    {
        std::string input{"World!"};
        std::vector<CodeInsertionByOffset> insertions{
            {.code = "Hey, ", .offset = 0}, {.code = "you. ", .offset = 7}, {.code = "Dummy. ", .offset = 0}};
        REQUIRE_THROWS_AS(apply_insertions(input, insertions), Tsepepe::BaseError);
        REQUIRE_THROWS_WITH(apply_insertions(input, std::move(insertions)),
                            "Code insertion: \"you. \", at offset 7, out of bounds");
    }
}

/**
 * @file        test_class_name_to_potential_header_file_name_regex.cpp
 * @brief       Tests the utility which converts a class name to a header file name regex.
 */

#include <regex>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils.hpp"

using namespace Tsepepe;

TEST_CASE("Converts class names to a header file name regex", "[utils][class_name_to_regex]")
{
    GIVEN("A class name")
    {
        auto class_name = GENERATE("SomeClass", "someClass", "some_class");

        WHEN("The regex is created for that class name")
        {
            auto regex{utils::class_name_to_header_file_regex(class_name)};

            THEN("It matches corresponding header file name")
            {
                auto matching_header_file_name = GENERATE("some_class.hpp",
                                                          "SomeClass.hpp",
                                                          "someClass.hpp",
                                                          "some_class.hh",
                                                          "SomeClass.h",
                                                          "someclass.h",
                                                          "someClass.hxx",
                                                          "Someclass.hh",
                                                          "some_class.hh");
                AND_THEN("It doesn't match any other header file name")
                {
                    REQUIRE(std::regex_match(class_name, regex));
                }
            }
        }
    }

    SECTION("From snake_cased class name")
    {
    }

    SECTION("From PascalCased class name")
    {
    }

    SECTION("From camelCase class name")
    {
    }
}

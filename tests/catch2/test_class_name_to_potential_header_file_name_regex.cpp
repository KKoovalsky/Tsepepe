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
    GIVEN("A single word class name")
    {
        auto class_name = GENERATE("masta", "Masta", "MASTA");

        WHEN("The regex is created for that class name")
        {
            auto regex{utils::class_name_to_header_file_regex(class_name)};

            THEN("It matches corresponding header file name")
            {
                auto matching_header_file_name = GENERATE("masta.hpp",
                                                          "MASTA.hpp",
                                                          "Masta.hpp",
                                                          "Masta.hh",
                                                          "masta.h",
                                                          "Masta.h",
                                                          "Masta.hxx",
                                                          "Masta.hh",
                                                          "masta.hh");

                INFO("Matching: " << matching_header_file_name << ", against regex: " << regex);
                REQUIRE(std::regex_match(matching_header_file_name, std::regex{regex, std::regex::icase}));
            }

            THEN("It matches not a not corresponding header file name")
            {
                auto non_matching_header_file_name = GENERATE("amasta.hpp", "mastaa.hpp", "Mastam.hh");
                REQUIRE_FALSE(std::regex_match(non_matching_header_file_name, std::regex{regex, std::regex::icase}));
            }
        }
    }

    GIVEN("A two word class name")
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
                INFO("Matching: " << matching_header_file_name << ", against regex: " << regex);
                REQUIRE(std::regex_match(matching_header_file_name, std::regex{regex, std::regex::icase}));
            }

            THEN("It matches not a not corresponding header file name")
            {
                auto non_matching_header_file_name = GENERATE("someclasss.hpp", "ssomeclass.h", "omecla.hxx");
                REQUIRE_FALSE(std::regex_match(non_matching_header_file_name, std::regex{regex, std::regex::icase}));
            }
        }
    }
}

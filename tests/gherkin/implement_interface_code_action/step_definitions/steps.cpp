/**
 * @file	steps.cpp
 * @brief	Cucumber step definitions for the implement interface code action tests.
 */
// clang-format off
#include <cucumber-cpp/generic.hpp>
#include <cucumber-cpp/autodetect.hpp>
// clang-format on

// using cucumber::ScenarioScope;

GIVEN("^Header file \"([^\"]*)\" with content$", (std::string file, std::string content))
{
}

WHEN("Implement interface code action is invoked for file \"([^\"]*)\" with line at cursor (\\d+)$",
     (std::string file, unsigned line_number))
{
}

THEN("^The result is$", (std::string result))
{
}


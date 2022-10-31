/**
 * @file        test_implement_interface_code_action.cpp
 * @brief       Tests the implement interface code action.
 */
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Implement interface code action is invoked", "[ImplementInterfaceCodeAction]")
{
    SECTION("Tries to find a file with the interface with specified name");

    SECTION(
        "Extracts the fully qualified override declarations from the interface, taking into account the current "
        "namespace nesting");
}

TEST_CASE("Matching interface file finder")
{
    SECTION("Picks the first matched symbol that is an interface");
}

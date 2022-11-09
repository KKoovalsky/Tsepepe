/**
 * @file        test_include_resolver.hpp
 * @brief       Tests the include resolver.
 */
#include <catch2/catch_test_macros.hpp>

TEST_CASE("A new include place is resolved", "[IncludeResolver]")
{
    SECTION("Resolves the top of the file if no include is found")
    {
    }

    SECTION("Resolves the top of the file, just below the comment header, if no include is found")
    {
    }

    SECTION("Resolves to two lines below the last <...> include, if no \"...\" includes are found")
    {
    }

    SECTION("Resolves to one line below the last \"...\" include")
    {
    }

    SECTION("Resolves nothing if the include is already found")
    {
    }
}

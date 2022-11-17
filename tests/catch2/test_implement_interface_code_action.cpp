/**
 * @file        test_implement_interface_code_action.cpp
 * @brief       Tests the implement interface code action.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "directory_tree.hpp"
#include "implement_interface_code_action.hpp"

using namespace Tsepepe;

TEST_CASE("Generate code that makes a class implement an interface", "[ImplementInterfaceCodeAction]")
{
    DirectoryTree directory_tree{"temp"};

    std::string error_message;
    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database{
        clang::tooling::CompilationDatabase::loadFromDirectory(
            COMPILATION_DATABASE_DIR, // Supplied within CMakeLists.txt
            error_message)};
    if (compilation_database == nullptr)
        throw std::runtime_error{"Failed to load compilation database from: " COMPILATION_DATABASE_DIR ": "
                                 + error_message};

    ImplementIntefaceCodeActionLibclangBased code_action{compilation_database};

    SECTION("Implements a basic interface")
    {
        GIVEN("An interface")
        {
            directory_tree.create_file("runnable.hpp",
                                       "struct Runnable\n"
                                       "{\n"
                                       "    virtual void run() = 0;\n"
                                       "    virtual int stop(unsigned timeout_ms) = 0;\n"
                                       "};\n");
            AND_GIVEN("A class definition")
            {
                std::string class_def{
                    "struct Maker\n"
                    "{\n"
                    "};\n"};

                WHEN("Implement interface code action is invoked")
                {
                    unsigned cursor_position_line = GENERATE(1, 2, 3);
                    auto result{code_action.apply(RootDirectory{"temp"},
                                                  FileContentConstRef{class_def},
                                                  InterfaceName{"Runnable"},
                                                  CursorPositionLine{cursor_position_line})};

                    THEN("New file content is returned")
                    {
                        REQUIRE(result == 
                                  "#include \"runnable.hpp\"\n"
                                  "struct Maker : Runnable\n"
                                  "{\n"
                                  "    void run() override;\n"
                                  "    int stop(unsigned int timeout_ms) override;\n"
                                  "};\n"
                        );
                    }
                }
            }
        }
    }

    SECTION("Implements a compound interface")
    {
    }
    SECTION("Properly resolves types nested within the interface")
    {
    }
    SECTION(
        "Properly resolves types nested within the same namespace the interface is defined, but outside of the "
        "interface")
    {
    }
    SECTION(
        "Properly resolves types nested within the same namespace the interface is defined, but outside of the "
        "interface, when the implementor is in the same namespace")
    {
    }
    SECTION("Error when interface not found")
    {
    }
    SECTION("Error when interface not found, but a normal class exists with the given name")
    {
    }
    SECTION("Does not load file from filesystem, but from standard input")
    {
    }
    SECTION("Implementor is a class with no base classes")
    {
    }
    SECTION("Implementor is a class with a single base class")
    {
    }
    SECTION("Implementor is a class with multiple base classes span over multiple lines")
    {
    }
    SECTION("Implementor is an empty class")
    {
    }
    SECTION("Implementor is an empty struct")
    {
    }
    SECTION("Implementor is a struct with mutliple methods")
    {
    }
    SECTION("Implementor is a class with mutliple public methods")
    {
    }
    SECTION("Implementor is a class with only a private section")
    {
    }
    SECTION("Implementor is a class with a single public section")
    {
    }
    SECTION("Does not add include if the interface already included")
    {
    }
    SECTION("Does not add base class specifier if already exists")
    {
    }
}

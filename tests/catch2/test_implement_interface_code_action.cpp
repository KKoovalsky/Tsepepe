/**
 * @file        test_implement_interface_code_action.cpp
 * @brief       Tests the implement interface code action.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

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
        GIVEN("An interface")
        {
            directory_tree.create_file("runnable.hpp",
                                       "struct Runnable\n"
                                       "{\n"
                                       "    virtual void run() = 0;\n"
                                       "    virtual int stop(unsigned timeout_ms) = 0;\n"
                                       "};\n");

            AND_GIVEN("Another interface")
            {

                directory_tree.create_file("printable.hpp",
                                           "struct Printable\n"
                                           "{\n"
                                           "    struct Error {};\n"
                                           "    virtual Error print(const char*, unsigned length) = 0;\n"
                                           "};\n");

                AND_GIVEN("A compound interface which comprises these two interfaces")
                {
                    directory_tree.create_file("runnable_and_printable.hpp",
                                               "#include \"runnable.hpp\"\n"
                                               "#include \"printable.hpp\"\n"
                                               "struct RunnableAndPrintable : Runnable, Printable\n"
                                               "{\n"
                                               "};\n");

                    AND_GIVEN("A class definition")
                    {
                        std::string class_def{
                            "struct Maker\n"
                            "{\n"
                            "};\n"};

                        WHEN("The compound interface is requested to be implemented")
                        {
                            unsigned cursor_position_line = GENERATE(1, 2, 3);
                            auto result{code_action.apply(RootDirectory{"temp"},
                                                          FileContentConstRef{class_def},
                                                          InterfaceName{"RunnableAndPrintable"},
                                                          CursorPositionLine{cursor_position_line})};

                            THEN("The class definition implements the compound interface")
                            {
                                std::string expected_result{
                                    "#include \"runnable_and_printable.hpp\"\n"
                                    "struct Maker : RunnableAndPrintable\n"
                                    "{\n"
                                    "    void run() override;\n"
                                    "    int stop(unsigned int timeout_ms) override;\n"
                                    "    Error print(const char *, unsigned int length) override;\n"
                                    "};\n"};

                                REQUIRE(result == expected_result);
                            }
                        }
                    }
                }
            }
        }
    }

    SECTION("Extends pretty big class")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "/**\n"
                " * @file        logger.hpp\n"
                " * @brief       Logger interface.\n"
                " * @author      Kacper Kowalski - kacper.s.kowalski@gmail.com\n"
                " */\n"
                "#ifndef LOGGER_HPP\n"
                "#define LOGGER_HPP\n"
                "\n"
                "enum class LogLevel\n"
                "{\n"
                "    debug,\n"
                "    info,\n"
                "    error,\n"
                "    raw\n"
                "};\n"
                "\n"
                "inline constexpr const char* log_level_to_string(LogLevel level)\n"
                "{\n"
                "    switch (level)\n"
                "    {\n"
                "    case LogLevel::debug:\n"
                "        return \"[DEBUG]\";\n"
                "    case LogLevel::info:\n"
                "        return \"[INFO]\";\n"
                "    case LogLevel::error:\n"
                "        return \"[ERROR]\";\n"
                "    default:\n"
                "        return \"\";\n"
                "    }\n"
                "};\n"
                "\n"
                "struct Logger\n"
                "{\n"
                "    virtual void log(LogLevel, const char* format) = 0;\n"
                "\n"
                "    virtual ~Logger() = default;\n"
                "};\n"
                "\n"
                "#endif /* LOGGER_HPP */\n"};

            directory_tree.create_file("logger.hpp", std::move(iface));

            AND_GIVEN("A class definition")
            {
                std::string class_def{
                    "/**\n"
                    " * @file        directory_tree.hpp\n"
                    " * @brief       Defines directory tree.\n"
                    " */\n"
                    "#ifndef DIRECTORY_TREE_HPP\n"
                    "#define DIRECTORY_TREE_HPP\n"
                    "\n"
                    "#include <filesystem>\n"
                    "\n"
                    "namespace Tsepepe\n"
                    "{\n"
                    "\n"
                    "class DirectoryTree\n"
                    "{\n"
                    "  public:\n"
                    "    explicit DirectoryTree(std::filesystem::path root);\n"
                    "    ~DirectoryTree();\n"
                    "\n"
                    "    std::filesystem::path create_file(std::filesystem::path relative_path_from_root, std::string "
                    "file_content);\n"
                    "    std::string load_file(std::filesystem::path relative_path_from_root);\n"
                    "\n"
                    "  private:\n"
                    "    std::filesystem::path root;\n"
                    "};\n"
                    "\n"
                    "} // namespace Tsepepe\n"
                    "\n"
                    "#endif /* DIRECTORY_TREE_HPP */\n"};

                WHEN("The interface is requested to be implemented")
                {
                    // FIXME: shall work for the line 25 as well (range() generates a range: [13, 25) )!
                    unsigned cursor_position_line = GENERATE(range(13, 25));
                    auto result{code_action.apply(RootDirectory{"temp"},
                                                  FileContentConstRef{class_def},
                                                  InterfaceName{"Logger"},
                                                  CursorPositionLine{cursor_position_line})};

                    THEN("The class definition implements the compound interface")
                    {
                        std::string expected_result{
                            "/**\n"
                            " * @file        directory_tree.hpp\n"
                            " * @brief       Defines directory tree.\n"
                            " */\n"
                            "#ifndef DIRECTORY_TREE_HPP\n"
                            "#define DIRECTORY_TREE_HPP\n"
                            "\n"
                            "#include <filesystem>\n"
                            "\n"
                            "#include \"logger.hpp\"\n"
                            "\n"
                            "namespace Tsepepe\n"
                            "{\n"
                            "\n"
                            "class DirectoryTree : public Logger\n"
                            "{\n"
                            "  public:\n"
                            "    explicit DirectoryTree(std::filesystem::path root);\n"
                            "    ~DirectoryTree();\n"
                            "\n"
                            "    std::filesystem::path create_file(std::filesystem::path relative_path_from_root, "
                            "std::string "
                            "file_content);\n"
                            "    std::string load_file(std::filesystem::path relative_path_from_root);\n"
                            "    void log(LogLevel, const char * format) override;\n"
                            "\n"
                            "  private:\n"
                            "    std::filesystem::path root;\n"
                            "};\n"
                            "\n"
                            "} // namespace Tsepepe\n"
                            "\n"
                            "#endif /* DIRECTORY_TREE_HPP */\n"};
                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }

    SECTION("Properly resolves types nested within the interface")
    {
        GIVEN("An interface returning some nested types")
        {
            std::string iface{
                "/**\n"
                " * @file        logger.hpp\n"
                " * @brief       Logger interface.\n"
                " * @author      Kacper Kowalski - kacper.s.kowalski@gmail.com\n"
                " */\n"
                "#ifndef LOGGER_HPP\n"
                "#define LOGGER_HPP\n"
                "\n"
                "struct Logger\n"
                "{\n"
                "    enum class LogLevel\n"
                "    {\n"
                "        debug,\n"
                "        info,\n"
                "        error,\n"
                "        raw\n"
                "    };\n"
                "\n"
                "    virtual void log(LogLevel, const char* format) = 0;\n"
                "\n"
                "    virtual ~Logger() = default;\n"
                "};\n"
                "\n"
                "#endif /* LOGGER_HPP */\n"};

            directory_tree.create_file("logger.hpp", std::move(iface));

            AND_GIVEN("A class")
            {
                std::string class_def{
                    "struct Yolo {\n"
                    "};\n"};

                WHEN("The interface is requested to be implemented")
                {
                    auto result{code_action.apply(RootDirectory{"temp"},
                                                  FileContentConstRef{class_def},
                                                  InterfaceName{"Logger"},
                                                  CursorPositionLine{1})};

                    THEN("The nested type is properly scope-shortened")
                    {
                        std::string expected_result{
                            "#include \"logger.hpp\"\n"
                            "struct Yolo : Logger {\n"
                            "    void log(LogLevel, const char * format) override;\n"
                            "};\n"};

                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }

    SECTION(
        "Interface is within a namespace and uses types from that namespace, but the implementor is in another "
        "namespace")
    {
        GIVEN("An interface nested within a namespace")
        {
            std::string iface{
                "namespace Interface\n"
                "{\n"
                "    struct ScanResult {};\n"
                "    struct Scanner\n"
                "    {\n"
                "        virtual ScanResult scan() = 0;\n"
                "        virtual ~Scanner() = default;\n"
                "    };\n"
                "};\n"};
            directory_tree.create_file("scanner.hpp", std::move(iface));

            AND_GIVEN("An about-to-be implementor")
            {
                std::string class_def{
                    "#include <string>\n"
                    "#include <vector>\n"
                    "\n"
                    "class Implementor\n"
                    "{\n"
                    "  public:\n"
                    "};\n"};

                WHEN("The interface is requested to be implemented")
                {
                    auto result{code_action.apply(RootDirectory{"temp"},
                                                  FileContentConstRef{class_def},
                                                  InterfaceName{"Scanner"},
                                                  CursorPositionLine{5})};

                    THEN("The types are properly resolved")
                    {
                        std::string expected_result{
                            "#include <string>\n"
                            "#include <vector>\n"
                            "\n"
                            "#include \"scanner.hpp\"\n"
                            "\n"
                            "class Implementor : public Interface::Scanner\n"
                            "{\n"
                            "  public:\n"
                            "    Interface::ScanResult scan() override;\n"
                            "};\n"};
                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
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

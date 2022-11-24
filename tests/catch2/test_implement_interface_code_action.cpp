/**
 * @file        test_implement_interface_code_action.cpp
 * @brief       Tests the implement interface code action.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "base_error.hpp"
#include "directory_tree.hpp"
#include "implement_interface_code_action.hpp"

using namespace Tsepepe;
namespace fs = std::filesystem;

TEST_CASE("Generate code that makes a class implement an interface", "[ImplementInterfaceCodeAction]")
{
    DirectoryTree directory_tree{"temp"};
    auto working_root_dir{directory_tree.get_root_absolute_path()};

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
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir,
                                                   .source_file_content = class_def,
                                                   .inteface_name = "Runnable",
                                                   .cursor_position_line = cursor_position_line})};

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
                            auto result{code_action.apply({.root_directory = "temp",
                                                           .source_file_path = working_root_dir,
                                                           .source_file_content = class_def,
                                                           .inteface_name = "RunnableAndPrintable",
                                                           .cursor_position_line = cursor_position_line})};

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
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir,
                                                   .source_file_content = class_def,
                                                   .inteface_name = "Logger",
                                                   .cursor_position_line = cursor_position_line})};

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
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir,
                                                   .source_file_content = class_def,
                                                   .inteface_name = "Logger",
                                                   .cursor_position_line = 1})};

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
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir,
                                                   .source_file_content = class_def,
                                                   .inteface_name = "Scanner",
                                                   .cursor_position_line = 5})};

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

    SECTION("Error when interface not found")
    {
        auto do_apply{[&]() {
            std::string class_def{"struct Yolo {};\n"};
            code_action.apply({.root_directory = "temp",
                               .source_file_path = working_root_dir,
                               .source_file_content = class_def,
                               .inteface_name = "Scanner",
                               .cursor_position_line = 1}

            );
        }};

        REQUIRE_THROWS_AS(do_apply(), Tsepepe::BaseError);
        REQUIRE_THROWS_WITH(do_apply(),
                            Catch::Matchers::ContainsSubstring("interface")
                                and Catch::Matchers::ContainsSubstring("found"));
    }

    SECTION("Error when interface not found, but a normal class exists with the given name")
    {
        std::string iface{
            "namespace Interface\n"
            "{\n"
            "    struct ScanResult {};\n"
            "    struct Scanner\n"
            "    {\n"
            "    };\n"
            "};\n"};
        directory_tree.create_file("scanner.hpp", std::move(iface));

        auto do_apply{[&]() {
            std::string class_def{"struct Yolo {};\n"};
            code_action.apply({.root_directory = "temp",
                               .source_file_path = working_root_dir,
                               .source_file_content = class_def,
                               .inteface_name = "Scanner",
                               .cursor_position_line = 1}

            );
        }};

        REQUIRE_THROWS_AS(do_apply(), Tsepepe::BaseError);
        REQUIRE_THROWS_WITH(do_apply(),
                            Catch::Matchers::ContainsSubstring("interface")
                                and Catch::Matchers::ContainsSubstring("found"));
    }

    SECTION("Implementor is a class with multiple base classes span over multiple lines")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "struct ScanResult {};\n"
                "struct Scanner\n"
                "{\n"
                "    virtual ScanResult scan() = 0;\n"
                "    virtual ~Scanner() = default;\n"
                "};\n"};
            directory_tree.create_file("scanner.hpp", std::move(iface));

            AND_GIVEN("Some classes to be extended")
            {
                std::string other_classes{
                    "struct One {};\n"
                    "struct Two {};\n"
                    "struct Three {};\n"};
                directory_tree.create_file("others.hpp", std::move(other_classes));

                AND_GIVEN("A class which inherits from multiple base classes")
                {
                    std::string class_def{
                        "#include \"others.hpp\"\n"
                        "\n"
                        "class Implementor : public One,\n"
                        "                    public Two,\n"
                        "                    public Three\n"
                        "{\n"
                        "};\n"};

                    WHEN("The interface is being implemented")
                    {
                        auto result{code_action.apply({.root_directory = "temp",
                                                       .source_file_path = working_root_dir,
                                                       .source_file_content = class_def,
                                                       .inteface_name = "Scanner",
                                                       .cursor_position_line = 4})};

                        THEN("The base-clause is properly extended")
                        {
                            std::string expected_result{
                                "#include \"others.hpp\"\n"
                                "#include \"scanner.hpp\"\n"
                                "\n"
                                "class Implementor : public One,\n"
                                "                    public Two,\n"
                                "                    public Three, public Scanner\n"
                                "{\n"
                                "public:\n"
                                "    ScanResult scan() override;\n"
                                "};\n"};

                            REQUIRE(result == expected_result);
                        }
                    }
                }
            }
        }
    }

    SECTION("Both interface and implementor files are nested deeply within the file system, and local files are used")
    {
        GIVEN("An interface in some nested directory")
        {
            std::string iface{
                "struct ScanResult {};\n"
                "struct Scanner\n"
                "{\n"
                "    virtual ScanResult scan() = 0;\n"
                "    virtual ~Scanner() = default;\n"
                "};\n"};

            directory_tree.create_file("include/interfaces/scanner.hpp", std::move(iface));

            AND_GIVEN("A local file to be included by the implementor")
            {
                directory_tree.create_file("include/lib/real/implementor/yolo/others.hpp",
                                           "struct One{};\n"
                                           "struct Two{};\n"
                                           "struct Three{};\n");

                AND_GIVEN("A class which is nested deeply in another directory")
                {
                    fs::path file_path{working_root_dir / "include" / "lib" / "real" / "implementor"
                                       / "implementor.hpp"};
                    std::string file_content{
                        "#include \"yolo/others.hpp\"\n"
                        "\n"
                        "class Implementor : public One,\n"
                        "                    public Two,\n"
                        "                    public Three\n"
                        "{\n"
                        "};\n"};

                    WHEN("The interface is being implemented")
                    {
                        auto result{code_action.apply({.root_directory = "temp",
                                                       .source_file_path = file_path,
                                                       .source_file_content = file_content,
                                                       .inteface_name = "Scanner",
                                                       .cursor_position_line = 4})};

                        THEN("No errors related to inclusion occurr")
                        {
                            std::string expected_result{
                                "#include \"yolo/others.hpp\"\n"
                                "#include \"scanner.hpp\"\n"
                                "\n"
                                "class Implementor : public One,\n"
                                "                    public Two,\n"
                                "                    public Three, public Scanner\n"
                                "{\n"
                                "public:\n"
                                "    ScanResult scan() override;\n"
                                "};\n"};

                            REQUIRE(result == expected_result);
                        }
                    }
                }
            }
        }
    }

    SECTION("Implementor is an empty inline class")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "struct Scanner\n"
                "{\n"
                "    virtual int scan() = 0;\n"
                "    virtual ~Scanner() = default;\n"
                "};\n"};
            directory_tree.create_file("scanner.hpp", std::move(iface));

            AND_GIVEN("A bare class")
            {
                std::string class_def{"class Yolo {};"};

                WHEN("The bare class is going to implement the interface")
                {
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir / "implementor.hpp",
                                                   .source_file_content = class_def,
                                                   .inteface_name = "Scanner",
                                                   .cursor_position_line = 1})};

                    THEN("The class is properly extended")
                    {
                        std::string expected_result{
                            "#include \"scanner.hpp\"\n"
                            "class Yolo : public Scanner {public:\n"
                            "    int scan() override;\n"
                            "};"};

                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }

    SECTION("Implementor is a struct with mutliple methods")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "#include <string>\n"
                "#include <vector>\n"
                "\n"
                "struct Interface\n"
                "{\n"
                "    virtual void do_stuff() = 0;\n"
                "};\n"};
            directory_tree.create_file("interface.hpp", std::move(iface));

            AND_GIVEN("An about-to-be implementor which is a struct with multiple methods")
            {
                std::string struct_{
                    "#include <utility>\n"
                    "#include <string_view>\n"
                    "\n"
                    "struct SomeStruct\n"
                    "{\n"
                    "    SomeStruct() = default;\n"
                    "    std::pair<double, std::string_view> yolo();\n"
                    "\n"
                    "    int bang(std::pair<double, std::string_view>) {\n"
                    "        return 0;"
                    "    }\n"
                    "};\n"};

                WHEN("The implement interface code action is invoked")
                {
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir / "implementor.hpp",
                                                   .source_file_content = struct_,
                                                   .inteface_name = "Interface",
                                                   .cursor_position_line = 8})};

                    THEN("The struct is properly extended")
                    {
                        std::string expected_result{
                            "#include <utility>\n"
                            "#include <string_view>\n"
                            "\n"
                            "#include \"interface.hpp\"\n"
                            "\n"
                            "struct SomeStruct : Interface\n"
                            "{\n"
                            "    SomeStruct() = default;\n"
                            "    std::pair<double, std::string_view> yolo();\n"
                            "\n"
                            "    int bang(std::pair<double, std::string_view>) {\n"
                            "        return 0;"
                            "    }\n"
                            "    void do_stuff() override;\n"
                            "};\n"};

                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }

    SECTION("Does not add include if the interface already included")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "#include <string>\n"
                "#include <vector>\n"
                "\n"
                "struct Interface\n"
                "{\n"
                "    virtual void do_stuff() = 0;\n"
                "};\n"};
            directory_tree.create_file("interface.hpp", std::move(iface));

            AND_GIVEN("An about-to-be implementor which has the interface header already included")
            {
                std::string struct_{
                    "#include \"interface.hpp\"\n"
                    "struct SomeStruct\n"
                    "{\n"
                    "};\n"};

                WHEN("The implement interface code action is invoked")
                {
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir / "implementor.hpp",
                                                   .source_file_content = struct_,
                                                   .inteface_name = "Interface",
                                                   .cursor_position_line = 2})};

                    THEN("The include stays as it is")
                    {
                        std::string expected_result{
                            "#include \"interface.hpp\"\n"
                            "struct SomeStruct : Interface\n"
                            "{\n"
                            "    void do_stuff() override;\n"
                            "};\n"};

                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }

    SECTION("Does not add base class specifier if already exists")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "#include <string>\n"
                "#include <vector>\n"
                "\n"
                "struct Interface\n"
                "{\n"
                "    virtual void do_stuff() = 0;\n"
                "};\n"};
            directory_tree.create_file("interface.hpp", std::move(iface));

            AND_GIVEN("An about-to-be implementor which has the base specifier already in place")
            {
                std::string struct_{
                    "#include \"interface.hpp\"\n"
                    "struct SomeStruct : Interface\n"
                    "{\n"
                    "};\n"};

                WHEN("The implement interface code action is invoked")
                {
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir / "implementor.hpp",
                                                   .source_file_content = struct_,
                                                   .inteface_name = "Interface",
                                                   .cursor_position_line = 2})};

                    THEN("The base-clause stays as it is")
                    {
                        std::string expected_result{
                            "#include \"interface.hpp\"\n"
                            "struct SomeStruct : Interface\n"
                            "{\n"
                            "    void do_stuff() override;\n"
                            "};\n"};

                        // FIXME: shall not expect 'include' to be in place. If there is no #include "interface.hpp"
                        // then we get a compilation error, but even with the compilation error, we shall not duplicate
                        // the base-specifier.
                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }

    SECTION("Error is raised when no class under cursor is found")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "#include <string>\n"
                "#include <vector>\n"
                "\n"
                "struct Interface\n"
                "{\n"
                "    virtual void do_stuff() = 0;\n"
                "};\n"};
            directory_tree.create_file("interface.hpp", std::move(iface));

            AND_GIVEN("A struct")
            {
                std::string struct_{
                    "#include <string>\n"
                    "#include <vector>\n"
                    "\n"
                    "struct SomeStruct\n"
                    "{\n"
                    "};\n"
                    "\n"
                    "\n"
                    "using String = std::string;\n"};

                WHEN(
                    "The implement interface code action is invoked with cursor under a line which is not a class "
                    "definition")
                {
                    unsigned line = GENERATE(values({1, 2, 3, 7, 8, 9, 15}));

                    THEN("An error is raised")
                    {
                        auto do_apply{[&]() {
                            code_action.apply({.root_directory = "temp",
                                               .source_file_path = working_root_dir / "implementor.hpp",
                                               .source_file_content = struct_,
                                               .inteface_name = "Interface",
                                               .cursor_position_line = line});
                        }};

                        REQUIRE_THROWS_AS(do_apply(), Tsepepe::BaseError);
                        REQUIRE_THROWS_WITH(do_apply(),
                                            Catch::Matchers::ContainsSubstring("cursor")
                                                and Catch::Matchers::ContainsSubstring("found"));
                    }
                }
            }
        }
    }

    SECTION("Extends the most deeply nested class under cursor")
    {
        GIVEN("An interface")
        {
            std::string iface{
                "#include <string>\n"
                "#include <vector>\n"
                "\n"
                "struct Interface\n"
                "{\n"
                "    virtual void do_stuff() = 0;\n"
                "};\n"};
            directory_tree.create_file("interface.hpp", std::move(iface));

            AND_GIVEN("Multiple nested structs")
            {
                std::string structs{
                    "struct Top\n"
                    "{\n"
                    "    struct Nested\n"
                    "    {\n"
                    "        struct EvenMoreNested\n"
                    "        {\n"
                    "            struct ShittyNesting\n"
                    "            {\n"
                    "            };\n"
                    "        };\n"
                    "    };\n"
                    "};\n"};

                WHEN("We extend the almost the most deeply nested struct")
                {
                    // FIXME: shall work for '10' as well.
                    unsigned line = GENERATE(values({5, 6}));
                    auto result{code_action.apply({.root_directory = "temp",
                                                   .source_file_path = working_root_dir / "implementor.hpp",
                                                   .source_file_content = structs,
                                                   .inteface_name = "Interface",
                                                   .cursor_position_line = line})};

                    THEN("The almost most deeply nested struct is extended")
                    {
                        std::string expected_result{
                            "#include \"interface.hpp\"\n"
                            "struct Top\n"
                            "{\n"
                            "    struct Nested\n"
                            "    {\n"
                            "        struct EvenMoreNested : Interface\n"
                            "        {\n"
                            "            void do_stuff() override;\n"
                            "            struct ShittyNesting\n"
                            "            {\n"
                            "            };\n"
                            "        };\n"
                            "    };\n"
                            "};\n"};

                        REQUIRE(result == expected_result);
                    }
                }
            }
        }
    }
}

/**
 * @file        test_pure_virtual_functions_extractor.cpp
 * @brief       Tests the pure virtual functions extractor.
 */
#include <filesystem>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include "directory_tree.hpp"

#include "clang_ast_fixtures.hpp"

#include "libclang_utils/pure_virtual_functions_extractor.hpp"

using namespace clang;

namespace PureVirtualFunctionsExtractorTest
{

struct SingleHeaderTestData
{
    std::string description;
    std::string header_file_content;
    std::string class_name;
    Tsepepe::OverrideDeclarations expected_result;
};

static inline auto make_class_matcher(const std::string& name)
{
    return ast_matchers::cxxRecordDecl(ast_matchers::hasName(name)).bind("class");
}

struct ClangAstClassRetrieverFixture
{
    ClangAstClassRetrieverFixture(const std::string& header_file_content, std::string class_name) :
        ast_fixture{header_file_content}, class_name{std::move(class_name)}
    {
    }

    const clang::CXXRecordDecl* retrieve() const
    {
        using namespace clang;
        auto matcher{make_class_matcher(class_name)};
        return ast_fixture.get_first_match<CXXRecordDecl>(matcher);
    };

    const clang::SourceManager& get_source_manager() const
    {
        return ast_fixture.get_source_manager();
    }

  private:
    Tsepepe::ClangSingleAstFixture ast_fixture;
    std::string class_name;
};

}; // namespace PureVirtualFunctionsExtractorTest

TEST_CASE("Extracts pure virtual functions from abstract class and converts them to overriding declarations",
          "[PureVirtualFunctionsExtractor]")
{
    using namespace Tsepepe;
    using namespace PureVirtualFunctionsExtractorTest;

    DirectoryTree dir_tree{"temp"};

    SECTION("For an interface defined within a single file")
    {
        auto [description, header_file_content, interface_name, expected_result] = GENERATE(values({
            SingleHeaderTestData{.description = "Extracts and converts a single pure virtual function",
                                 .header_file_content = "struct Interface\n"
                                                        "{\n"
                                                        "    virtual void run(unsigned int) = 0;\n"
                                                        "    virtual ~Interface() = default;\n"
                                                        "};\n",
                                 .class_name = "Interface",
                                 .expected_result = {"void run(unsigned int) override;"}},
            SingleHeaderTestData{
                .description = "Extracts and converts multiple pure virtual functions",
                .header_file_content = "#include <string>\n"
                                       "#include <vector>\n"
                                       "struct Iface\n"
                                       "{\n"
                                       "    virtual int run(double d) = 0;\n"
                                       "    virtual std::string gimme(const std::vector<std::string>& some_list) = 0;\n"
                                       "    virtual ~Iface() = default;\n"
                                       "};\n",
                .class_name = "Iface",
                .expected_result = {"int run(double d) override;",
                                    "std::string gimme(const std::vector<std::string> & some_list) override;"}},
            SingleHeaderTestData{.description =
                                     "Extracts and converts pure virtual functions from a class in a namespace",
                                 .header_file_content = "namespace Yolo{\n"
                                                        "struct SomeIface\n"
                                                        "{\n"
                                                        "    virtual float gimme(int) = 0;\n"
                                                        "    virtual ~SomeIface() = default;\n"
                                                        "};\n"
                                                        "}\n",
                                 .class_name = "SomeIface",
                                 .expected_result = {"float gimme(int) override;"}},
            SingleHeaderTestData{.description = "Extracts and converts pure virtual functions from a nested class",
                                 .header_file_content = "struct Yolo\n"
                                                        "{\n"
                                                        "    struct SomeIfacez\n"
                                                        "    {\n"
                                                        "        virtual float gimme(int) = 0;\n"
                                                        "        virtual ~SomeIfacez() = default;\n"
                                                        "    };\n"
                                                        "};\n",
                                 .class_name = "SomeIfacez",
                                 .expected_result = {"float gimme(int) override;"}},
        }));

        INFO(description);

        ClangAstClassRetrieverFixture fixture{header_file_content, interface_name};
        auto result{pure_virtual_functions_to_override_declarations(
            fixture.retrieve(), "SomeImplementer", fixture.get_source_manager())};
        REQUIRE_THAT(result, Catch::Matchers::Equals(expected_result));
    }

    SECTION("Shortifies types defined in the same namespace")
    {
        DirectoryTree dir_tree{"temp"};
        GIVEN("A file with some common type, defined within a common namespace")
        {
            dir_tree.create_file("common.hpp",
                                 "namespace Namespace {\n"
                                 "struct External\n"
                                 "{\n"
                                 "    struct Nested\n"
                                 "    {\n"
                                 "    };\n"
                                 "};\n"
                                 "}\n");
            AND_GIVEN("An interface defined within the common namespace. The interface uses the common type.")
            {
                auto path{dir_tree.create_file("ifejs.hpp",
                                               "#include \"common.hpp\"\n"
                                               "namespace Namespace {\n"
                                               "struct Ifejs\n"
                                               "{\n"
                                               "    virtual External::Nested gimme(External::Nested) = 0;\n"
                                               "    virtual ~Ifejs() = default;\n"
                                               "};\n"
                                               "}\n")};

                WHEN("Override declarations are collected from the interface")
                {
                    ClangAstFixture fixture{COMPILATION_DATABASE_DIR, {path}};
                    using namespace clang;
                    auto matcher{make_class_matcher("Ifejs")};
                    auto iface{fixture.get_first_match<CXXRecordDecl>(matcher)};
                    auto result{pure_virtual_functions_to_override_declarations(
                        iface, "Namespace::SomeImplementer", fixture.get_source_manager())};

                    THEN("Types within the same namespace are shortified")
                    {
                        REQUIRE_THAT(result,
                                     Catch::Matchers::Equals(
                                         OverrideDeclarations{"External::Nested gimme(External::Nested) override;"}));
                    }
                }
            }
        }
    }

    SECTION("Extracts and converts pure virtual functions from a compound interface")
    {
        DirectoryTree dir_tree{"temp"};

        GIVEN("One interface")
        {
            dir_tree.create_file("runnable.hpp",
                                 "#include <string>\n"
                                 "struct Runnable\n"
                                 "{\n"
                                 "    virtual void run(std::string name) = 0;\n"
                                 "    virtual ~Runnable() = default;\n"
                                 "};\n");

            AND_GIVEN("Another interface")
            {
                dir_tree.create_file("printer.hpp",
                                     "#include <string>\n"
                                     "#include <vector>\n"
                                     "struct Printer\n"
                                     "{\n"
                                     "    struct PrintingResult\n"
                                     "    {\n"
                                     "    };\n"
                                     "\n"
                                     "    virtual PrintingResult print(std::vector<std::string> stuff) = 0;\n"
                                     "    virtual ~Printer() = default;\n"
                                     "};\n");

                AND_GIVEN("A compound interface created from these two interfaces")
                {
                    auto path{dir_tree.create_file("runnable_printer.hpp",
                                                   "#include \"printer.hpp\"\n"
                                                   "#include \"runnable.hpp\"\n"
                                                   "struct RunnablePrinter : Runnable, Printer\n"
                                                   "{\n"
                                                   "    virtual ~RunnablePrinter() = default;\n"
                                                   "};\n")};

                    WHEN("Override declarations are obtained from the compound interface")
                    {

                        ClangAstFixture fixture{COMPILATION_DATABASE_DIR, {path}};
                        using namespace clang;
                        auto matcher{make_class_matcher("RunnablePrinter")};
                        auto iface{fixture.get_first_match<CXXRecordDecl>(matcher)};
                        auto result{pure_virtual_functions_to_override_declarations(
                            iface, "YoloImplementor", fixture.get_source_manager())};

                        THEN("The pure virtual functions from the comprising interfaces are collected")
                        {
                            REQUIRE_THAT(result,
                                         Catch::Matchers::UnorderedEquals(OverrideDeclarations{
                                             "void run(std::string name) override;",
                                             "PrintingResult print(std::vector<std::string> stuff) override;"}));
                        }
                    }
                }
            }
        }
    }

    SECTION("Extracts and converts pure virtual functions from a complex compound interface")
    {
        DirectoryTree dir_tree{"temp"};

        GIVEN("One interface")
        {
            dir_tree.create_file("display.hpp",
                                 "#include <vector>\n"
                                 "struct Display\n"
                                 "{\n"
                                 "    using Bytes = std::vector<unsigned char>;\n"
                                 "    virtual void display(const Bytes&) = 0;\n"
                                 "    virtual ~Display() = default;\n"
                                 "};\n");

            AND_GIVEN("Another interface")
            {
                dir_tree.create_file("printer.hpp",
                                     "#include <string>\n"
                                     "#include <vector>\n"
                                     "struct Printer\n"
                                     "{\n"
                                     "    struct PrintingResult\n"
                                     "    {\n"
                                     "    };\n"
                                     "\n"
                                     "    virtual PrintingResult print(std::vector<std::string> stuff) = 0;\n"
                                     "    virtual ~Printer() = default;\n"
                                     "};\n");

                AND_GIVEN("A compound interface created from these two interfaces")
                {
                    dir_tree.create_file("displaying_printer.hpp",
                                         "#include \"display.hpp\"\n"
                                         "#include \"printer.hpp\"\n"
                                         "struct DisplayingPrinter : Display, Printer\n"
                                         "{\n"
                                         "    virtual ~DisplayingPrinter() = default;\n"
                                         "};\n");

                    AND_GIVEN("One more interfaces which extends the compound interface")
                    {
                        auto path{dir_tree.create_file("dumpable_displaying_printer.hpp",
                                                       "#include \"displaying_printer.hpp\"\n"
                                                       "struct DumpableDisplayingPrinter : DisplayingPrinter\n"
                                                       "{\n"
                                                       "    virtual void dump() = 0;\n"
                                                       "    virtual ~DumpableDisplayingPrinter() = default;\n"
                                                       "};\n")};

                        WHEN("Override declarations are obtained from the extender of the compound interface")
                        {
                            ClangAstFixture fixture{COMPILATION_DATABASE_DIR, {path}};
                            using namespace clang;
                            auto matcher{make_class_matcher("DumpableDisplayingPrinter")};
                            auto iface{fixture.get_first_match<CXXRecordDecl>(matcher)};
                            auto result{pure_virtual_functions_to_override_declarations(
                                iface, "YoloImplementor", fixture.get_source_manager())};

                            THEN("All the pure virtual functions are collected")
                            {
                                REQUIRE_THAT(result,
                                             Catch::Matchers::UnorderedEquals(OverrideDeclarations{
                                                 "void display(const Bytes &) override;",
                                                 "PrintingResult print(std::vector<std::string> stuff) override;",
                                                 "void dump() override;"}));
                            }
                        }
                    }
                }
            }
        }
    }
}

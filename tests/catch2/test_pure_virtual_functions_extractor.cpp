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
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include "directory_tree.hpp"

#include "clang_ast_fixtures.hpp"

#include "libclang_utils/pure_virtual_functions_extractor.hpp"

namespace fs = std::filesystem;
using namespace clang;

namespace PureVirtualFunctionsExtractorTest
{
struct HeaderFile
{
    fs::path path;
    std::string content;
};

struct TestData
{
    std::string description;
    std::vector<HeaderFile> header_file_content;
    std::string interface_name;
    std::string header_file_with_interface;
    std::vector<std::string> expected_result;
};

struct ClangAstClassRetrieverFixture
{
    ClangAstClassRetrieverFixture(const std::string& header_file_content, std::string class_name) :
        ast_fixture{header_file_content}, class_name{std::move(class_name)}
    {
    }

    const clang::CXXRecordDecl* retrieve() const
    {
        using namespace clang;
        auto matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasName(class_name)).bind("class")};

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

    SECTION("Extracts and converts a single pure virtual function")
    {
        GIVEN("An interface")
        {
            std::string iface_def{
                "struct Interface\n"
                "{\n"
                "    virtual void run(unsigned int) = 0;\n"
                "    virtual ~Interface() = default;\n"
                "};\n"};

            WHEN("Override declarations are extracted")
            {
                ClangAstClassRetrieverFixture fixture{iface_def, "Interface"};
                auto result{
                    pure_virtual_functions_to_override_declarations(fixture.retrieve(), fixture.get_source_manager())};

                THEN("The proper result is obtained")
                {
                    REQUIRE_THAT(result,
                                 Catch::Matchers::Equals(OverrideDeclarations{"void run(unsigned int) override;"}));
                }
            }
        }
    }
}

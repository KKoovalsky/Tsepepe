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

struct ClangSingleAstFixture
{
    explicit ClangSingleAstFixture(const std::string& iface_name, const std::string& header_file_content) :
        ast_unit{tooling::buildASTFromCode(header_file_content)}
    {
        auto iface_matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasName(iface_name), ast_matchers::hasDefinition())
                               .bind("iface")};
        auto matches{clang::ast_matchers::match(iface_matcher, ast_unit->getASTContext())};
        if (matches.size() == 0)
            throw std::runtime_error{"Failed to find " + iface_name
                                     + "within the file content: " + header_file_content};

        const auto& first_match{matches[0]};
        auto node{first_match.getNodeAs<CXXRecordDecl>("class")};
        if (node == nullptr)
            throw std::runtime_error{"Failed to get node for " + iface_name
                                     + "within the file content: " + header_file_content};
        iface_node = node;
    }

    const CXXRecordDecl* get_iface_node() const
    {
        return iface_node;
    }

    const SourceManager& get_source_manager() const
    {
        return ast_unit->getSourceManager();
    }

  private:
    std::unique_ptr<ASTUnit> ast_unit;
    const CXXRecordDecl* iface_node{nullptr};
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
                ClangSingleAstFixture fixture{"Interface", iface_def};
                auto result{pure_virtual_functions_to_override_declarations(fixture.get_iface_node(),
                                                                            fixture.get_source_manager())};

                THEN("The proper result is obtained")
                {
                    REQUIRE_THAT(result,
                                 Catch::Matchers::Equals(OverrideDeclarations{"void run(unsigned int) override;"}));
                }
            }
        }
    }
}

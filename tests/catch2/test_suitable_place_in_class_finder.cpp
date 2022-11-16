/**
 * @file        test_suitable_place_in_class_finder.cpp
 * @brief       Tests the suitable place in class finder.
 */

#include <string>
#include <vector>

#include "ostream_printers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/Tooling.h>

#include "libclang_utils/suitable_place_in_class_finder.hpp"

using namespace Tsepepe;

struct SuitablePlaceInClassFinderTestData
{
    std::string description;
    std::string header_file_content;
    std::string class_name;
    SuitablePublicMethodPlaceInCppFile expected_result;

    auto operator<=>(const SuitablePlaceInClassFinderTestData&) const = default;
};

TEST_CASE("Finds suitable place in classes to put a new public function declaration", "[SuitablePlaceInClassFinder]")
{
    static std::initializer_list<SuitablePlaceInClassFinderTestData> test_data{
        SuitablePlaceInClassFinderTestData{.description = "In a class with a single public section",
                                           .header_file_content = "class Yolo\n"
                                                                  "{\n"
                                                                  "public:\n"
                                                                  "    Yolo() = default;\n"
                                                                  "    void gimme();\n"
                                                                  "};\n",
                                           .class_name = "Yolo",
                                           .expected_result = {.offset = 61}},
        SuitablePlaceInClassFinderTestData{.description = "In a weirdly formatted class with a single public section",
                                           .header_file_content = "class Yolo\n"
                                                                  "{\n"
                                                                  "public:\n"
                                                                  "    Yolo() = default;\n"
                                                                  "    void gimme();};"
                                                                  "\n",
                                           .class_name = "Yolo",
                                           .expected_result = {.offset = 60}},
        // SuitablePlaceInClassFinderTestData{
        //     .description = "In a class with private methods",
        //     .header_file_content = "class Maka\n"
        //                            "{\n"
        //                            "private:\n"
        //                            "    void gimme();\n"
        //                            "    void sijek();\n"
        //                            "};\n",
        //     .class_name = "Maka",
        //     .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 2, .is_public_section_needed = true}},
        // SuitablePlaceInClassFinderTestData{.description = "In a class with public and private section, in that
        // order",
        //                                    .header_file_content = "\n"
        //                                                           "class Bumm\n"
        //                                                           "{\n"
        //                                                           "public:\n"
        //                                                           "private:\n"
        //                                                           "};\n"
        //                                                           "\n",
        //                                    .class_name = "Bumm",
        //                                    .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 4}},
        // SuitablePlaceInClassFinderTestData{.description = "In a class with private and public section, in that
        // order",
        //                                    .header_file_content = "\n"
        //                                                           "\n"
        //                                                           "class Bumm\n"
        //                                                           "{\n"
        //                                                           "private:\n"
        //                                                           "public:\n"
        //                                                           "};\n"
        //                                                           "\n",
        //                                    .class_name = "Bumm",
        //                                    .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 6}},
        //
        // SuitablePlaceInClassFinderTestData{
        //     .description = "In a class with public, private and one more public section, in that order",
        //     .header_file_content = "\n"
        //                            "class Masta\n"
        //                            "{\n"
        //                            "public:\n"
        //                            "\n"
        //                            "private:\n"
        //                            "\n"
        //                            "public:\n"
        //                            "};\n"
        //                            "\n",
        //     .class_name = "Masta",
        //     .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 4}},
        // SuitablePlaceInClassFinderTestData{.description = "In an empty struct",
        //                                    .header_file_content = "struct Yolo\n"
        //                                                           "{\n"
        //                                                           "};\n",
        //                                    .class_name = "Yolo",
        //                                    .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 2}},
        // SuitablePlaceInClassFinderTestData{.description = "In a struct with properties only",
        //                                    .header_file_content = "#include <string>\n"
        //                                                           "\n"
        //                                                           "struct Yolo\n"
        //                                                           "{\n"
        //                                                           "    std::string s;\n"
        //                                                           "    unsigned n;\n"
        //                                                           "};\n",
        //                                    .class_name = "Yolo",
        //                                    .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 4}},
        SuitablePlaceInClassFinderTestData{.description = "In a struct with few public methods",
                                           .header_file_content = "\n"
                                                                  "struct Yolo\n"
                                                                  "{\n"
                                                                  "    Yolo() {}\n"
                                                                  "\n"
                                                                  "    ~Yolo()\n"
                                                                  "    {\n"
                                                                  "    }\n"
                                                                  "\n"
                                                                  "    void gimme();\n"
                                                                  "};\n",
                                           .class_name = "Yolo",
                                           .expected_result = SuitablePublicMethodPlaceInCppFile{.offset = 73}},
        SuitablePlaceInClassFinderTestData{.description = "In a struct with the last public method being a definition",
                                           .header_file_content = "\n"
                                                                  "struct Yolo\n"
                                                                  "{\n"
                                                                  "    Yolo() {}\n"
                                                                  "\n"
                                                                  "    ~Yolo()\n"
                                                                  "    {\n"
                                                                  "    }\n"
                                                                  "\n"
                                                                  "    void gimme()\n"
                                                                  "    {\n"
                                                                  "    }\n"
                                                                  "};\n",
                                           .class_name = "Yolo",
                                           .expected_result = SuitablePublicMethodPlaceInCppFile{.offset = 84}},
        SuitablePlaceInClassFinderTestData{.description = "In a struct with methods being followed by properties",
                                           .header_file_content = "\n"
                                                                  "struct Yolo\n"
                                                                  "{\n"
                                                                  "    Yolo() {}\n"
                                                                  "\n"
                                                                  "    ~Yolo()\n"
                                                                  "    {\n"
                                                                  "    }\n"
                                                                  "\n"
                                                                  "    void gimme();\n"
                                                                  "\n"
                                                                  "private:\n"
                                                                  "    unsigned v{0};\n"
                                                                  "};\n",
                                           .class_name = "Yolo",
                                           .expected_result = SuitablePublicMethodPlaceInCppFile{.offset = 73}},
        // SuitablePlaceInClassFinderTestData{
        //     .description = "In an empty class",
        //     .header_file_content = "\n"
        //                            "class Yolo\n"
        //                            "{\n"
        //                            "};\n",
        //     .class_name = "Yolo",
        //     .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 3, .is_public_section_needed = true}},
        // SuitablePlaceInClassFinderTestData{
        //     .description = "In an empty class with the opening bracket on the same line as the class name",
        //     .header_file_content = "\n"
        //                            "class Yolo {\n"
        //                            "\n"
        //                            "};\n",
        //     .class_name = "Yolo",
        //     .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 2, .is_public_section_needed = true}},
        // SuitablePlaceInClassFinderTestData{
        //     .description = "In a class with private section only",
        //     .header_file_content = "\n"
        //                            "class Yolo \n"
        //                            "{\n"
        //                            "  private:\n"
        //                            "    unsigned i;\n"
        //                            "    float f;\n"
        //                            "\n"
        //                            "};\n",
        //     .class_name = "Yolo",
        //     .expected_result = SuitablePublicMethodPlaceInCppFile{.line = 3, .is_public_section_needed = true}},
    };
    auto [description, header_file_content, class_name, expected_result] = GENERATE(values(test_data));
    INFO(description);

    using namespace clang;
    auto class_matcher{ast_matchers::cxxRecordDecl(ast_matchers::hasName(class_name)).bind("class")};
    auto ast_unit{tooling::buildASTFromCode(header_file_content, "header.hpp")};
    auto match_result{clang::ast_matchers::match(class_matcher, ast_unit->getASTContext())};

    REQUIRE(match_result.size() > 0);

    const auto& first_match{match_result[0]};
    auto node{first_match.getNodeAs<CXXRecordDecl>("class")};

    REQUIRE(node != nullptr);

    auto result{
        find_suitable_place_in_class_for_public_method(header_file_content, node, ast_unit->getSourceManager())};
    REQUIRE(result == expected_result);
}

// struct Yolo {     Yolo() {}      ~Yolo()     {     }      void gimme(); };
//  struct Yolo {     Yolo() {}      ~Yolo()     {     }      void gimme()     {     } };

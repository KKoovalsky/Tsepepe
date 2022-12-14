/**
 * @file        test_self_deleting_file.cpp
 * @brief       Tests the self deleting file.
 */
#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

#include "self_deleting_file.hpp"

TEST_CASE("Self deleting file creates and deletes itself", "[SelfDeletingFile]")
{
    using namespace Tsepepe;
    namespace fs = std::filesystem;

    auto temp_file_path{fs::temp_directory_path() / "temp.txt"};
    REQUIRE_FALSE(fs::exists(temp_file_path));

    SECTION("Creates itself on construction")
    {
        {
            SelfDeletingFile f{temp_file_path, ""};
            REQUIRE(fs::exists(temp_file_path));
        }

        SECTION("Deletes itself on destruction")
        {
            REQUIRE_FALSE(fs::exists(temp_file_path));
        }
    }

    SECTION("Writes the content to the file")
    {
        SelfDeletingFile f{temp_file_path, "some yolo content"};

        std::ifstream ifs{temp_file_path};
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        REQUIRE(buffer.str() == "some yolo content");
    }

    SECTION("Resolves to an absolute path")
    {
        SelfDeletingFile f{"some_yolo.txt", ""};
        REQUIRE(f.is_absolute());
    }
}

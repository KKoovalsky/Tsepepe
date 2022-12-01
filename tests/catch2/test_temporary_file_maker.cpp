/**
 * @file        test_temporary_file_maker.cpp
 * @brief       Tests the temporary file maker.
 */
#include <catch2/catch_test_macros.hpp>

#include <filesystem>

#include "temporary_file_maker.hpp"

TEST_CASE("Temporary files are created and deleted", "[TemporaryFileMaker]")
{
    namespace fs = std::filesystem;
    using namespace Tsepepe;

    auto temp_dir{fs::temp_directory_path()};
    auto temp_file_path{temp_dir / "basta.cpp"};

    SECTION("Temporary file is created, when full path is specified")
    {
        {
            auto f{make_temporary_source_file(temp_file_path, "some_id", "")};
            REQUIRE(fs::exists(temp_dir / ".tsepepe_basta.cpp"));
        }

        SECTION("The temporary file is deleted on destruction")
        {
            REQUIRE_FALSE(fs::exists(temp_dir / ".tsepepe_basta.cpp"));
        }
    }

    SECTION("Temporary file is created, when only the parent directory is specified")
    {
        auto f{make_temporary_source_file(temp_dir, "yolo_id", "")};
        REQUIRE(fs::exists(temp_dir / ".tsepepe_yolo_id_temp.hpp"));
    }
}

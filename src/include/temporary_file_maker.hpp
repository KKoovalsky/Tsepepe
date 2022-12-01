/**
 * @file        temporary_file_maker.hpp
 * @brief       Defines the temporary file maker.
 */
#ifndef TEMPORARY_FILE_MAKER_HPP
#define TEMPORARY_FILE_MAKER_HPP

#include "self_deleting_file.hpp"

#include <filesystem>

namespace Tsepepe
{

/**
 * Makes a temporary source file in the same directory as the path specified. If the path is a directory then the
 * temporary file will be created in that directory, and its filename will be ".tsepepe_<id>_temp.cpp".
 */
inline SelfDeletingFile
make_temporary_source_file(std::filesystem::path path, const std::string& id, const std::string& content)
{
    namespace fs = std::filesystem;

    fs::path temp_file_path;
    if (not fs::is_directory(path))
    {
        // Assume the path is a path to a file.
        std::string fname{".tsepepe_" + path.filename().string()};
        temp_file_path = path.parent_path() / std::move(fname);
    } else
    {
        std::string fname{".tsepepe_" + id + "_temp.hpp"};
        temp_file_path = path / fname;
    }

    return SelfDeletingFile{temp_file_path, content};
}

} // namespace Tsepepe

#endif /* TEMPORARY_FILE_MAKER_HPP */

/**
 * @file        self_deleting_file.hpp
 * @brief       Defines the SelfDeletingFile, which uses RAII to create and delete itself.
 */
#ifndef SELF_DELETING_FILE_HPP
#define SELF_DELETING_FILE_HPP

#include <filesystem>
#include <fstream>

namespace Tsepepe
{
struct SelfDeletingFile : std::filesystem::path
{
    SelfDeletingFile(std::filesystem::path p) : std::filesystem::path{std::move(p)}
    {
        std::ofstream{*this};
    }

    ~SelfDeletingFile()
    {
        std::filesystem::remove(*this);
    }
};
} // namespace Tsepepe

#endif /* SELF_DELETING_FILE_HPP */

/**
 * @file        self_deleting_file.hpp
 * @brief       Defines the SelfDeletingFile, which uses RAII to create and delete itself.
 */
#ifndef SELF_DELETING_FILE_HPP
#define SELF_DELETING_FILE_HPP

#include <filesystem>
#include <fstream>
#include <string>

namespace Tsepepe
{
struct SelfDeletingFile : std::filesystem::path
{
    SelfDeletingFile(const SelfDeletingFile&) = delete;
    SelfDeletingFile& operator=(const SelfDeletingFile&) = delete;
    SelfDeletingFile(SelfDeletingFile&&) = default;
    SelfDeletingFile& operator=(SelfDeletingFile&&) = default;

    SelfDeletingFile(std::filesystem::path p, const std::string& content) : std::filesystem::path{std::move(p)}
    {
        std::ofstream{*this} << content;
    }

    ~SelfDeletingFile()
    {
        std::filesystem::remove(*this);
    }
};
} // namespace Tsepepe

#endif /* SELF_DELETING_FILE_HPP */

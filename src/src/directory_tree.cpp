/**
 * @file	directory_tree.cpp
 * @brief	Implements the DirectoryTree.
 */

#include <filesystem>
#include <fstream>

#include "directory_tree.hpp"

namespace fs = std::filesystem;

Tsepepe::DirectoryTree::DirectoryTree(std::filesystem::path root) : root(std::move(root))
{
    fs::create_directory(this->root);
}

Tsepepe::DirectoryTree::~DirectoryTree()
{
    fs::remove_all(root);
}

std::string Tsepepe::DirectoryTree::load_file(std::filesystem::path relative_path_from_root)
{
    auto path{root / relative_path_from_root};
    std::ifstream ifs{path};
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

std::filesystem::path Tsepepe::DirectoryTree::create_file(std::filesystem::path relative_path_from_root,
                                                          std::string file_content)
{
    auto path{root / relative_path_from_root};
    fs::create_directories(path.parent_path());
    std::ofstream{path} << file_content;
    return fs::absolute(path).lexically_normal();
}

std::filesystem::path Tsepepe::DirectoryTree::get_root_absolute_path() const
{
    return fs::absolute(root).lexically_normal();
}

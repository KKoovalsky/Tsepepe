/**
 * @file	directory_tree.cpp
 * @brief	Implements the DirectoryTree.
 */

#include <filesystem>
#include <fstream>
#include <iostream>

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

void Tsepepe::DirectoryTree::create_file(std::filesystem::path relative_path_from_root, std::string file_content)
{
    auto path{root / relative_path_from_root};
    std::cout << path << std::endl;
    fs::create_directories(path.parent_path());
    std::ofstream{path} << file_content;
}


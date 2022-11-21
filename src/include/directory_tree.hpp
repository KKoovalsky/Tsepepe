/**
 * @file        directory_tree.hpp
 * @brief       Defines directory tree.
 */
#ifndef DIRECTORY_TREE_HPP
#define DIRECTORY_TREE_HPP

#include <filesystem>

namespace Tsepepe
{

class DirectoryTree
{
  public:
    explicit DirectoryTree(std::filesystem::path root);
    ~DirectoryTree();

    std::filesystem::path create_file(std::filesystem::path relative_path_from_root, std::string file_content);
    std::string load_file(std::filesystem::path relative_path_from_root);

    std::filesystem::path get_root_absolute_path() const;

  private:
    std::filesystem::path root;
};

} // namespace Tsepepe

#endif /* DIRECTORY_TREE_HPP */

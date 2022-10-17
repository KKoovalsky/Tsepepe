/**
 * @file	compilation_database_parser.cpp
 * @brief	Defines the compilation database parsing utility.
 */

#include "utils.hpp"

namespace Tsepepe::utils
{

std::unique_ptr<clang::tooling::CompilationDatabase>
parse_compilation_database(const std::filesystem::path& directory_with_compilation_database)
{
    parse_and_validate_path(directory_with_compilation_database);
    auto comp_db_path{directory_with_compilation_database / "compile_commands.json"};
    parse_and_validate_path(comp_db_path);

    std::string err;
    auto r{clang::tooling::CompilationDatabase::loadFromDirectory(directory_with_compilation_database.string(), err)};
    if (!r)
    {
        std::string msg;
        msg.reserve(100 + err.size());
        msg.append("ERROR: While parsing compilation database, from clang::tooling::CompilationDatabase: ");
        msg.append(std::move(err));
        throw Tsepepe::Error{std::move(msg)};
    }

    return r;
}

} // namespace Tsepepe::utils

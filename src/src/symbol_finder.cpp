/**
 * @file        symbol_finder.cpp
 * @brief       Implementation of the symbol finder.
 */

#include "symbol_finder.hpp"

#include <boost/process.hpp>

// --------------------------------------------------------------------------------------------------------------------
// Private helpers declaration
// --------------------------------------------------------------------------------------------------------------------
namespace fs = std::filesystem;

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::vector<Tsepepe::SymbolMatch> Tsepepe::find_symbol(const std::filesystem::path& root, SymbolName)
{
    return {};
}

// --------------------------------------------------------------------------------------------------------------------
// Private helpers definition
// --------------------------------------------------------------------------------------------------------------------
static std::vector<fs::path> ripgrep_for_symbol(const fs::path& root, const std::string& class_name)
{
    std::string class_definition_regex{"(^|\\s+)(struct|class)\\s+" + class_name + "\\b"};
    std::string command{"rg " + class_definition_regex + " " + root.string() + " -l"};

    std::vector<fs::path> result;

    using namespace boost::process;
    ipstream pipe_stream;
    child c{std::move(command), std_out > pipe_stream};
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        result.emplace_back(std::move(line));
    c.wait();

    return result;
}


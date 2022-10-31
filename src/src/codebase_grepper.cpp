/**
 * @file	codebase_grepper.cpp
 * @brief	Implements the codebase grepping.
 */

#include "codebase_grepper.hpp"
#include "base_error.hpp"

#include <boost/process.hpp>
#include <filesystem>
#include <string>

// --------------------------------------------------------------------------------------------------------------------
// Helper declarations
// --------------------------------------------------------------------------------------------------------------------
namespace fs = std::filesystem;

//! Parses ripgrep --vimgrep output and returns a tuple: matching file, line number and beginning column number.
static std::tuple<fs::path, unsigned, unsigned> parse_line(const std::string& line);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
std::vector<Tsepepe::GrepMatch> Tsepepe::codebase_grep(RootDirectory root_dir_alias, EcmaScriptPattern pattern)
{
    const auto& root_dir{root_dir_alias.get()};
    std::string command{"rg " + pattern.get() + " " + root_dir.string() + " --vimgrep -t cpp"};

    std::vector<Tsepepe::GrepMatch> result;

    using namespace boost::process;

    ipstream pipe_stream;
    child c{std::move(command), std_out > pipe_stream};

    auto current_path{fs::current_path()};
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
    {
        auto [relative_path, line_number, column_begin] = parse_line(line);
        result.emplace_back(
            Tsepepe::GrepMatch{.path = current_path / relative_path, .line = line_number, .column = column_begin});
    }

    c.wait();

    return result;
}

// --------------------------------------------------------------------------------------------------------------------
// Helper definitions
// --------------------------------------------------------------------------------------------------------------------
static std::tuple<fs::path, unsigned, unsigned> parse_line(const std::string& line)
{
    auto beg{0};
    auto end{line.find(':')};
    if (end == std::string::npos)
        throw Tsepepe::BaseError{"Unexpected ripgrep output, when parsing path of a file with the match"};

    fs::path relative_path{line.substr(beg, end)};

    ++end;
    beg = end;
    end = line.find(':', beg);
    if (end == std::string::npos)
        throw Tsepepe::BaseError{"Unexpected ripgrep output, when parsing line number with the match"};

    auto line_number_str{line.substr(beg, end - beg)};
    auto line_number{std::stoul(line_number_str)};

    ++end;
    beg = end;
    end = line.find(':', beg);
    if (end == std::string::npos)
        throw Tsepepe::BaseError{"Unexpected ripgrep output, when parsing column begin number where the match begins"};

    auto column_begin_str{line.substr(beg, end - beg)};
    auto column_begin{std::stoul(column_begin_str)};

    return {relative_path, line_number, column_begin};
}

/**
 * @file	file_grepper.cpp
 * @brief	Implements the file grepper.
 */

#include "file_grepper.hpp"

#include <boost/process.hpp>

// --------------------------------------------------------------------------------------------------------------------
// Helper declarations
// --------------------------------------------------------------------------------------------------------------------

//! Parses ripgrep --vimgrep output and returns a tuple: matching file, line number and beginning column number.
// static std::tuple<fs::path, unsigned, unsigned> parse_line(const std::string& line);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
Tsepepe::FileGrepMatches Tsepepe::grep_file(const std::string& file_content, RustRegexPattern pattern)
{
    std::string command{"rg " + pattern.get() + " --line-number -"};

    using namespace boost::process;

    ipstream output;
    opstream input;
    child c{std::move(command), std_out > output, std_in < input};

    input << file_content;
    input.close();
    input.pipe().close();

    FileGrepMatches result;
    result.reserve(8);
    std::string line;

    while (output && std::getline(output, line) && !line.empty())
    {
        auto colon_idx{line.find(':')};
        if (colon_idx == std::string::npos)
            continue;
        auto line_number_str{line.substr(0, colon_idx)};
        result.emplace_back(std::stoul(line_number_str));
    }

    c.wait();

    return result;
}


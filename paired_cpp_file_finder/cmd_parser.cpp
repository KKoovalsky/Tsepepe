/**
 * @file	cmd_parser.cpp
 * @brief	Implements the command line parsing for the C++ paired file finder.
 */
#include <exception>
#include <filesystem>
#include <iostream>
#include <string_view>

#include "cmd_parser.hpp"

namespace fs = std::filesystem;

// --------------------------------------------------------------------------------------------------------------------
// Helper declaration
// --------------------------------------------------------------------------------------------------------------------
struct Error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

static void print_usage(int argc, const char** argv);
static void validate_path_exists(std::string_view name, const fs::path&);
static void validate_path_in_directory(const fs::path& root, const fs::path& potentially_nested);
static void validate_is_cpp_file(const fs::path& file);

//! Turns relative paths to absolute, and fixes all "..". For absolute paths fixes ".." only.
static fs::path normalize(const fs::path&);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
namespace Tsepepe::PairedCppFileFinder
{

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv)
{
    if (std::string first_argument(argv[1]); first_argument == "-h" or first_argument == "--help")
    {
        print_usage(argc, argv);
        return ReturnCode{0};
    }

    if (argc != 3)
    {
        std::cerr << "ERROR: Invalid number of arguments!\n" << std::endl;
        print_usage(argc, argv);
        return ReturnCode{1};
    }

    try
    {
        validate_path_exists("Project root directory", argv[1]);
        auto project_root{normalize(argv[1])};
        fs::path cpp_file_path{argv[2]};
        if (cpp_file_path.is_absolute())
        {
            validate_path_exists("C++ file", cpp_file_path);
            cpp_file_path = normalize(cpp_file_path);
            validate_path_in_directory(project_root, cpp_file_path);
        } else // C++ file path is relative ...
        {
            cpp_file_path = project_root / cpp_file_path;
            validate_path_exists("C++ file", cpp_file_path);
        }
        validate_is_cpp_file(cpp_file_path);
        return Input{.project_directory = project_root, .cpp_file = cpp_file_path};
    } catch (const Error& e)
    {
        std::cerr << e.what() << std::endl;
        return ReturnCode{1};
    }
}

}; // namespace Tsepepe::PairedCppFileFinder

// --------------------------------------------------------------------------------------------------------------------
// Helper definition
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv)
{
    auto program_path{argv[0]};
    std::cout << "USAGE:\n\t" << program_path << " PROJECT_ROOT_DIR CPP_FILE\n\n";
    std::cout
        << "DESCRIPTION:\n\tTries to find a corresponding (paired) C++ file for CPP_FILE, under PROJECT_ROOT_DIR.\n\t"
           "Paired C++ files are files with the same stem, e.g.: some_file.cpp/some_file.hpp.\n\n\t"
           "If the input file is a header file, then source file is tried to be found. "
           "Otherwise, if the input file is a source file, then a header file is tried to be found.\n\n\t"
           "Firstly, tries to find the paired file in the same directory the input file is located. "
           "Then, traverses the directories recursively under PROJECT_ROOT_DIR. "
           "If finds multiple matches, then outputs them, each in a separate line.\n\n"
        << std::endl;
    std::cout << "NOTE:\n\tCPP_FILE must be in located in the child directory of the PROJECT_ROOT_DIR.\n" << std::endl;
    std::cout << "EXAMPLE:\n\t1. </root/dir/to/project>\n"
                 "\t\t\t|\n"
                 "\t\t\t|---- some_dir\n"
                 "\t\t\t\t\t|---- foo.hpp\n"
                 "\t\t\t\t\t|---- foo.cpp\n"
                 "\tWhen invoking:\n"
                 "\t\t./paired_cpp_file_finder /root/dir/to/project some_dir/foo.hpp\n"
                 "\tor:\n"
                 "\t\t./paired_cpp_file_finder /root/dir/to/project /root/dir/to/project/some_dir/foo.hpp\n"
                 "\n\tThe result outputted to stdout is:\n"
                 "\t\t/root/dir/to/project/some_dir/foo.cpp\n";
    std::cout << "\n\n\t2. </root/dir/to/project>\n"
                 "\t\t\t|\n"
                 "\t\t\t|---- some_dir1\n"
                 "\t\t\t\t\t|---- foo.cpp\n"
                 "\t\t\t|---- some_dir2\n"
                 "\t\t\t\t\t|---- foo.hpp\n"
                 "\t\t\t|---- some_dir3\n"
                 "\t\t\t\t\t|---- foo.hpp\n"
                 "\tWhen invoking:\n"
                 "\t\t./paired_cpp_file_finder /root/dir/to/project some_dir1/foo.cpp\n"
                 "\n\tThe result outputted to stdout is:\n"
                 "\t\t/root/dir/to/project/some_dir2/foo.hpp\n"
                 "\t\t/root/dir/to/project/some_dir3/foo.hpp\n";
    std::cout << "\n\n\t3. </root/dir/to/project>\n"
                 "\t\t\t|\n"
                 "\t\t\t|---- some_dir1\n"
                 "\t\t\t\t\t|---- foo.cpp\n"
                 "\t\t\t\t\t|---- foo.hpp\n"
                 "\t\t\t|---- some_dir2\n"
                 "\t\t\t\t\t|---- foo.hpp\n"
                 "\t\t\t|---- some_dir3\n"
                 "\t\t\t\t\t|---- foo.hpp\n"
                 "\tWhen invoking:\n"
                 "\t\t./paired_cpp_file_finder /root/dir/to/project some_dir1/foo.cpp\n"
                 "\n\tThe result outputted to stdout is:\n"
                 "\t\t/root/dir/to/project/some_dir1/foo.hpp\n"
              << std::endl;
}

static void validate_path_exists(std::string_view name, const fs::path& path)
{
    if (not fs::exists(path))
    {
        auto msg{(std::ostringstream{} << "ERROR: " << name << ": " << path << " does not exist!").str()};
        throw Error{std::move(msg)};
    }
}

static fs::path normalize(const fs::path& path)
{
    return fs::absolute(path).lexically_normal();
}

static void validate_path_in_directory(const fs::path& root, const fs::path& potentially_nested)
{
    auto relative{fs::relative(potentially_nested, root)};
    if (relative.empty() or relative.string().starts_with(".."))
    {
        auto msg{
            (std::ostringstream{} << "ERROR: Path: " << potentially_nested << " is not inside: " << root << "!").str()};
        throw Error{std::move(msg)};
    }
}

static void validate_is_cpp_file(const fs::path& file)
{
    static constexpr std::string_view allowed_extensions[]{".cpp", ".cxx", ".cc", ".h", ".hpp", ".hh"};
    auto match_it{std::find(std::begin(allowed_extensions), std::end(allowed_extensions), file.extension())};
    if (match_it == std::end(allowed_extensions))
    {
        auto msg{(std::ostringstream{} << "ERROR: File: " << file << " is not a C++ file!").str()};
        throw Error{std::move(msg)};
    }
}

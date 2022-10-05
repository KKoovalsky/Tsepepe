/**
 * @file	cmd_parser.cpp
 * @brief	Implements the command line parsing for the C++ paired file finder.
 */
#include <iostream>

#include "cmd_parser.hpp"

// --------------------------------------------------------------------------------------------------------------------
// Helper declaration
// --------------------------------------------------------------------------------------------------------------------
static void print_usage(int argc, const char** argv);

// --------------------------------------------------------------------------------------------------------------------
// Public stuff
// --------------------------------------------------------------------------------------------------------------------
namespace Tsepepe::PairedCppFileFinder
{

std::variant<Input, ReturnCode> parse_cmd(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::cout << "ERROR: Invalid number of arguments!" << std::endl;
        print_usage(argc, argv);
        return ReturnCode{1};
    }

    return Input{};
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

# Tsepepe

A collection of C++ refactoring tools (WIP), provided as binaries. 

- [Function definition generator](#function-definition-generator).
- [Paired C++ file finder](#paired-c++-file-finder)
- [Implementor maker](#implementor-maker)

# Build requirements

* GCC 12.1.0+ (clang-14 has been tried, with no luck - fails on compiling the mix of std::filesystem iterators
with views)
* CMake 3.22
* `libclang-14-dev`, `libllvm-14-dev` or `libclang-15-dev`, `libllvm-15-dev`
* `liboost-1.74`+
* `ripgrep`

## Building and installing

```
mkdir build
cd build/
cmake -DCMAKE_INSTALL_PREFIX=<where/to/install/the/binaries/> ..
cmake --build .
cmake --install .
# The binaries will be found under the directory: ${CMAKE_INSTALL_PREFIX}/bin.
```

## The tools

### Function definition generator

Allows to generate the function definition from a function declaration. Unfortunately, it needs compilation
database of the code, because it works on top of the libclang's C++ AST traversal.

It is used in such a way:
```
tsepepe_function_definition_generator                    \
    <path to directory with compilation database>        \
    <path to the C++ file with declaration>              \
    <cursor position line begin>                         \
    [optional cursor position line end]
```

When a selection is considered then the range [cursor position line begin; cursor position line end] is taken as the
selected range. In case of no selection, _cursor position line begin_ shall be the current position of the cursor.

For multiline declaration expects the first line with the declaration.

**Example:**

Having a header file called _some\_header.hpp_, with content:
```
namespace Yolo
{
class SomeClass
{
    unsigned int foo() const;
};
}
```
And compilation database in the current directory; when invoking:
```
tsepepe_function_definition_generator . some_header.hpp 5 # The declaration of foo() is in the 5th line.
```
The tool will output:
```
unsigned int Yolo::SomeClass::foo() const
```

**The generation rules:** (should be compliant with the C++ standard, but, please, create an issue in case something
is missing)
- All the namespace names, and nested class names will be preserved in the signature. It applies both to the function
name and the return type, which might be a custom type.
- Skips `virtual`, `override`, `static`, ... and other specifiers, which shall not appear in the definition.
- Keeps `const` and `noexcept`, `noexcept(<bool>)`.
- Skips attributes (`[[nodiscard]]`, ...).
- Keeps the ref-qualifier: `&` or `&&`.
- Skips default parameters.

### Paired C++ file finder

Invoke it like that:
```
tsepepe_paired_cpp_file_finder                                          \
    <project root directory>                                            \
    <path to the C++ file for which the paired file will be found>
```

Tries to find a corresponding (paired) C++ file for the project found under the specified root.
Paired C++ files are files with the same stem, e.g.: _some\_file.cpp/some\_file.hpp_.

**Note:** The input C++ file must be located in the child directory of the project root. The path may be absolute,
or relative to the project root directory.

If the input file is a header file, then source file is tried to be found. Otherwise, if the input file is a source 
file, then a header file is tried to be found.

Firstly, tries to find the paired file in the same directory the input file is located. Then, traverses the directories 
recursively under project root. If finds multiple matches, then outputs them, each in a separate line. It means, that
if the paired file is located in the same directory, it is returned straight away. Otherwise, the whole project 
directory is traversed to find a corresponding file. In case multiple matches are found, each of them is outputted to
a separate line.

Allowed extensions are:
```
.cpp, .cxx, .cc, .h, .hpp, .hh, .hxx.
```

Some examples:

1.

    </root/dir/to/project>
                |
                |---- some_dir
                                |---- foo.hpp
                                |---- foo.cpp

When invoking:

    paired_cpp_file_finder /root/dir/to/project some_dir/foo.hpp

or:

    paired_cpp_file_finder /root/dir/to/project /root/dir/to/project/some_dir/foo.hpp

The result outputted to stdout is:

    /root/dir/to/project/some_dir/foo.cpp

---

2.

    </root/dir/to/project>
                |
                |---- some_dir1
                                |---- foo.cpp
                |---- some_dir2
                                |---- foo.hpp
                |---- some_dir3
                                |---- foo.hpp

When invoking:

    paired_cpp_file_finder /root/dir/to/project some_dir1/foo.cpp

The result outputted to stdout is:

    /root/dir/to/project/some_dir2/foo.hpp
    /root/dir/to/project/some_dir3/foo.hpp

---

3.

    </root/dir/to/project>
                |
                |---- some_dir1
                                |---- foo.cpp
                                |---- foo.hpp
                |---- some_dir2
                                |---- foo.hpp
                |---- some_dir3
                                |---- foo.hpp

When invoking:

    paired_cpp_file_finder /root/dir/to/project some_dir1/foo.cpp

The result outputted to stdout is:

    /root/dir/to/project/some_dir1/foo.hpp

### Implementor maker

Makes a class/struct implementor of an interface, with the name that is provided as a parameter. 
The project's C++ source and header files are traversed to find the interface with the specified name. 
The line number of the current cursor position is used to find the potential implementor of the interface. Thus it 
means, that the current cursor position must be within the potential implementor body.
Takes also the entire file content as a parameter, and returns the new file content, with all the pieces of code added 
that make the class/struct the implementor of the interface, what means that:

* the proper `#include` statement is added,
* the base-clause is created, or extended, with the interface qualified name,
* the pure virtual functions are put to the class body, marked `override`.

The code shall properly compile after the applying the changes. The only exception is that the 
`#include "<path>"` is not validated. The filename, where the interface is defined, is put as `<path>`. It might be
that manual adjustment is needed, to properly resolve the include path, to avoid `file not found` error.

Invoke it like that:
```
tsepepe_implementor maker                                               \
    <project root directory>                                            \
    <path to directory with compilation database>                       \
    <path to the C++ file which will contain the implementor>           \
    <content of the C++ file which will contain the implementor>        \
    <the interface name>                                                \
    <line number where the cursor is located within the file with the potential implementor>
```

**Example:**

Having a project under path `<PROJECT_ROOT>`, and an interface defined within a file `src/include/yolo_interface.hpp`,
with content:

    namespace Tsepepe
    {
    struct YoloInterface
    {
        virtual void do_stuff() = 0;
        virtual ~YoloInterface() = default;
    };
    } // namespace Tsepepe

When the tool is called like that:

    tsepepe_implementor_maker
            <PROJECT_ROOT>/build               # The path to the directory with the compile_commands.json
            <PROJECT_ROOT>                     # The project root directory
            <PROJECT_ROOT>/src/implementor.hpp # Path to the file with the potential implementor
            'struct Implementor { };'          # The source file content
            YoloInterface                      # The inteface name.
            1                                  # The first line contains the Implementor definition.

The output will be:

    #include "yolo_interface.hpp"
    struct Implementor : Tsepepe::YoloInterface {
        void do_stuff() override;
    };

## Testing

Requirements:

1. Run: `pip3 install -r tests/gherkin/requirements.txt`.
2. Install `bear` tool.
3. `cucumber` command line tool. On Ubuntu can be installed with `apt`. It is a ruby tool, thus `gem` can also be used
for that.
4. `catch2` C++ testing library, version 3+.

Run:

```
cmake -DTSEPEPE_ENABLE_TESTING=ON ..
cmake --build . && ctest -LE long_running       # Will skip leak checking, which takes few minutes with valgrind.
```

To run all the tests, with leak checking included:
```
ctest
```

The tests are written in Gherkin, driven by `behave`.

## TODO

1. Extract method.
2. Extract function.
3. Extract lambda.
4. Introduce parameter.
5. Introduce lambda parameter.
6. Introduce property.
7. Extract interface?
8. Turn inline definition into out-of-line definition.

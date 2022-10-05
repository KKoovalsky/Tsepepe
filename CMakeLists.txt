cmake_minimum_required(VERSION 3.22)

project(Tsepepe C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TSEPEPE_ENABLE_TESTING "Enable testing of this project" OFF)

find_package(LLVM 14 REQUIRED)
find_package(Clang REQUIRED)

add_compile_options(-Wall)

add_subdirectory(function_definition_generator)
add_subdirectory(paired_cpp_file_finder)

if(TSEPEPE_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

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

find_package(LLVM REQUIRED)
if(LLVM_VERSION_MAJOR LESS 14)
    message(FATAL_ERROR "LLVM >= 14 is required")
endif()
find_package(Clang REQUIRED)

if(NOT TARGET clangTooling)
    # clangTooling may be either provided as individual libraries or as a bundled libclang-cpp lib
    find_library(CLANGTOOLING_LIBRARY NAMES clangTooling clang-cpp)
    add_library(clangTooling SHARED IMPORTED)
    set_target_properties(clangTooling PROPERTIES IMPORTED_LOCATION ${CLANGTOOLING_LIBRARY})
else()
    message(STATUS "Tsepepe: found clangTooling bundled with Clang package")
endif()

add_compile_options(-Wall)

include(cmake/dependencies.cmake)
add_subdirectory(src)

if(TSEPEPE_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

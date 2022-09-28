cmake_minimum_required(VERSION 3.22)

project(CppTinyRefactor C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM 14 REQUIRED)
find_package(Clang REQUIRED)

add_executable(test test.cpp)

# target_compile_definitions(test PRIVATE ${LLVM_DEFINITIONS})
# target_link_libraries(test PRIVATE )

target_include_directories(test PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(test PRIVATE LLVMSupport clangTooling)

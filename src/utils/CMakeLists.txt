add_library(tsepepe_utils STATIC 
    compilation_database_parser.cpp
    filesystem_utils.cpp
    cmd_utils.cpp
    clang_ast_utils.cpp
)
target_include_directories(tsepepe_utils PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(tsepepe_utils PUBLIC ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_utils PUBLIC clangTooling)

add_executable(tsepepe_full_class_name_expander tool.cpp expander.cpp cmd_parser.cpp)

target_include_directories(tsepepe_full_class_name_expander PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_full_class_name_expander PRIVATE 
    LLVM LLVMSupport clangTooling tsepepe_utils)


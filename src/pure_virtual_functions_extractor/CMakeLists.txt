add_executable(tsepepe_pure_virtual_functions_extractor tool.cpp extractor.cpp cmd_parser.cpp)

target_include_directories(tsepepe_pure_virtual_functions_extractor PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_pure_virtual_functions_extractor PRIVATE 
    LLVM LLVMSupport clangTooling tsepepe_utils)


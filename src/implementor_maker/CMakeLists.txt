add_executable(tsepepe_implementor_maker tool.cpp cmd_parser.cpp)

target_include_directories(tsepepe_implementor_maker PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_implementor_maker PRIVATE
    LLVM LLVMSupport clangTooling tsepepe_utils tsepepe_lib)

install(TARGETS tsepepe_implementor_maker)

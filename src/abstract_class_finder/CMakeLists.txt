add_executable(tsepepe_abstract_class_finder tool.cpp cmd_parser.cpp finder.cpp)

target_include_directories(tsepepe_abstract_class_finder PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_abstract_class_finder PRIVATE 
    LLVMSupport clangTooling tsepepe_utils Boost::headers range-v3::range-v3)

target_compile_options(tsepepe_abstract_class_finder PRIVATE -Wno-deprecated-enum-enum-conversion)

install(TARGETS tsepepe_abstract_class_finder)

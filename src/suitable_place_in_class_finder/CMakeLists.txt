add_executable(tsepepe_suitable_place_in_class_finder tool.cpp finder.cpp cmd_parser.cpp)

target_include_directories(tsepepe_suitable_place_in_class_finder PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_suitable_place_in_class_finder PRIVATE 
    LLVMSupport clangTooling Boost::headers tsepepe_utils)

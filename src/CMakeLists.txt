find_program(RIPGREP rg REQUIRED)

find_package(Boost 1.74 REQUIRED COMPONENTS headers)
ProvideRangesV3()
ProvideNamedType()
find_program(RIPGREP rg REQUIRED)

add_subdirectory(utils)
add_subdirectory(function_definition_generator)
add_subdirectory(paired_cpp_file_finder)
add_subdirectory(abstract_class_finder)
add_subdirectory(pure_virtual_functions_extractor)
add_subdirectory(suitable_place_in_class_finder)
add_subdirectory(full_class_name_expander)

add_library(tsepepe_lib STATIC
    src/implement_interface_code_action.cpp
    src/codebase_grepper.cpp
    src/file_grepper.cpp
    src/directory_tree.cpp
    src/include_statement_place_resolver.cpp
    src/libclang_utils/misc_utils.cpp
    src/libclang_utils/suitable_place_in_class_finder.cpp
    src/libclang_utils/pure_virtual_functions_extractor.cpp
    src/libclang_utils/full_function_declaration_expander.cpp
    src/libclang_utils/base_specifier_resolver.cpp
)
target_include_directories(tsepepe_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(tsepepe_lib PUBLIC NamedType)
target_include_directories(tsepepe_lib PUBLIC ${LLVM_INCLUDE_DIR})
target_link_libraries(tsepepe_lib PRIVATE LLVMSupport clangTooling Boost::headers)

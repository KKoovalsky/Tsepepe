find_package(Catch2 3 REQUIRED)

add_executable(tsepepe_lib_unit_test
    test_codebase_grepper.cpp
    test_implement_interface_code_action.cpp
    test_suitable_place_in_class_finder.cpp
    test_pure_virtual_functions_extractor.cpp
    test_full_function_declaration_expander.cpp
    test_include_statement_place_resolver.cpp
    test_base_specifier_resolver.cpp
    test_code_insertions_applier.cpp
    test_multiple_function_definitions_generator.cpp
    test_self_deleting_file.cpp
)

target_link_libraries(tsepepe_lib_unit_test Catch2::Catch2WithMain tsepepe_lib)
target_compile_definitions(tsepepe_lib_unit_test PRIVATE -DCOMPILATION_DATABASE_DIR="${CMAKE_BINARY_DIR}")

add_test(NAME tsepepe_lib_unit_test COMMAND $<TARGET_FILE:tsepepe_lib_unit_test>)

find_program(VALGRIND valgrind)
if(NOT VALGRIND)
    message(WARNING "'valgrind' not found! Leak-checking tests will not be added.")
else()
    add_test(NAME tsepepe_lib_leak_check_unit_test
             COMMAND valgrind --leak-check=full $<TARGET_FILE:tsepepe_lib_unit_test>)
    set_tests_properties(tsepepe_lib_leak_check_unit_test PROPERTIES LABELS long_running)
endif()

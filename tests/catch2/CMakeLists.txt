find_package(Catch2 3 REQUIRED)

add_executable(tsepepe_lib_unit_test
    test_codebase_grepper.cpp
    test_implement_interface_code_action.cpp
    test_suitable_place_in_class_finder.cpp
    test_pure_virtual_functions_extractor.cpp
)

target_link_libraries(tsepepe_lib_unit_test Catch2::Catch2WithMain tsepepe_lib)
target_compile_definitions(tsepepe_lib_unit_test PRIVATE -DCOMPILATION_DATABASE_DIR="${CMAKE_BINARY_DIR}")

find_program(VALGRIND valgrind)

if(NOT VALGRIND)
    message(WARNING "'valgrind' not found! Tests of the utils will be run without a leak checker.")
    add_test(NAME tsepepe_lib_unit_test COMMAND $<TARGET_FILE:tsepepe_lib_unit_test>)
else()
    add_test(NAME tsepepe_lib_unit_test COMMAND valgrind --leak-check=full $<TARGET_FILE:tsepepe_lib_unit_test>)
endif()

find_package(Catch2 3 REQUIRED)

add_executable(utils_unit_tests test_class_name_to_potential_header_file_name_regex.cpp)
target_link_libraries(utils_unit_tests Catch2::Catch2WithMain tsepepe_utils)

find_program(VALGRIND valgrind)

if(NOT VALGRIND)
    message(WARNING "'valgrind' not found! Tests of the utils will be run without a leak checker.")
    add_test(NAME utils_unit_tests COMMAND $<TARGET_FILE:utils_unit_tests>)
else()
    add_test(NAME utils_unit_tests COMMAND valgrind --leak-check=full $<TARGET_FILE:utils_unit_tests>)
endif()

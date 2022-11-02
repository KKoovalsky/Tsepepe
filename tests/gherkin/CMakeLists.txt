# ######################################################################################################################
# Functions
# ######################################################################################################################
function(AddToolTest name)

    add_test(
        NAME tsepepe_test_${name}
        COMMAND ${BEHAVE} -D tool_path=$<TARGET_FILE:tsepepe_${name}> ${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    set(help_regex "USAGE.*DESCRIPTION")

    set(help_printed_test tsepepe_test_${name}_help_printed_on_demand)
    add_test(NAME ${help_printed_test} COMMAND $<TARGET_FILE:tsepepe_${name}> -h)
    set_tests_properties(${help_printed_test} PROPERTIES PASS_REGULAR_EXPRESSION "${help_regex}")

    set(test_return_code_zero_when_help_requested tsepepe_test_${name}_zero_return_code_when_help_requested)
    add_test(NAME ${test_return_code_zero_when_help_requested} COMMAND $<TARGET_FILE:tsepepe_${name}> -h)

    set(test_help_printed_test_on_wrong_args_specified tsepepe_test_${name}_help_printed_when_args_wrong)
    add_test(NAME ${test_help_printed_test_on_wrong_args_specified} COMMAND $<TARGET_FILE:tsepepe_${name}>)
    set_tests_properties(${test_help_printed_test_on_wrong_args_specified} PROPERTIES PASS_REGULAR_EXPRESSION
                                                                                      "${help_regex}")

    set(test_return_code_non_zero_for_wrong_args tsepepe_test_${name}_non_zero_return_code_on_wrong_args)
    add_test(NAME ${test_return_code_non_zero_for_wrong_args} COMMAND $<TARGET_FILE:tsepepe_${name}>)
    set_tests_properties(${test_return_code_non_zero_for_wrong_args} PROPERTIES WILL_FAIL TRUE)

endfunction()

# ! AddCppCucumberTest: creates a cucumber test with 'name', which must match the subdirectory with the test.
function(AddCppCucumberTest name)

    set(test_name tsepepe_cucumber_cpp_${name}_test)
    add_executable(${test_name} ${name}/step_definitions/steps.cpp)
    target_link_libraries(${test_name} tsepepe_lib cucumber-cpp)
    target_compile_definitions(${test_name} PRIVATE -DCOMPILATION_DATABASE_DIR="${CMAKE_BINARY_DIR}")

    set(wire_server_starter ${test_name}_wire_server_starter)
    set(wire_server_stopper ${test_name}_wire_server_stopper)
    set(wire_server ${test_name}_wire_server)
    set(wire_server_pid_file ${CMAKE_CURRENT_BINARY_DIR}/${wire_server}.pid)
    set(daemonizer ${CMAKE_CURRENT_LIST_DIR}/daemonizer.py)
    set(daemon_name ${wire_server})

    add_test(NAME ${wire_server_starter} COMMAND ${daemonizer} ${daemon_name} ${wire_server_pid_file} start
                                                 $<TARGET_FILE:${test_name}>)
    add_test(NAME ${wire_server_stopper} COMMAND ${daemonizer} ${daemon_name} ${wire_server_pid_file} stop)
    add_test(
        NAME ${test_name}
        COMMAND cucumber ${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    set_tests_properties(${wire_server_starter} PROPERTIES FIXTURES_SETUP ${wire_server})
    set_tests_properties(${wire_server_stopper} PROPERTIES FIXTURES_CLEANUP ${wire_server})
    set_tests_properties(${test_name} PROPERTIES FIXTURES_REQUIRED ${wire_server})

endfunction()

# ######################################################################################################################
# Main script
# ######################################################################################################################
find_program(BEHAVE behave REQUIRED)

AddToolTest(function_definition_generator)
AddToolTest(paired_cpp_file_finder)
AddToolTest(abstract_class_finder)
AddToolTest(pure_virtual_functions_extractor)
AddToolTest(suitable_place_in_class_finder)
AddToolTest(full_class_name_expander)

find_program(CUCUMBER cucumber REQUIRED)
ProvideCucumberCpp()

AddCppCucumberTest(implement_interface_code_action)

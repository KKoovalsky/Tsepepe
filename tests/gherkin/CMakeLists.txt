################################################################################
# Functions
################################################################################
function(AddToolTest name)

    add_test(
        NAME tsepepe_test_${name}
        COMMAND ${BEHAVE} -D tool_path=$<TARGET_FILE:tsepepe_${name}> ${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    set(help_regex "USAGE.*DESCRIPTION")

    set(help_printed_test tsepepe_test_${name}_help_printed_on_demand)
    add_test(
        NAME ${help_printed_test}
        COMMAND $<TARGET_FILE:tsepepe_${name}> -h)
    set_tests_properties(${help_printed_test} PROPERTIES PASS_REGULAR_EXPRESSION "${help_regex}")

    set(test_return_code_zero_when_help_requested tsepepe_test_${name}_zero_return_code_when_help_requested)
    add_test(
        NAME ${test_return_code_zero_when_help_requested}
        COMMAND $<TARGET_FILE:tsepepe_${name}> -h)

    set(test_help_printed_test_on_wrong_args_specified tsepepe_test_${name}_help_printed_when_args_wrong)
    add_test(
        NAME ${test_help_printed_test_on_wrong_args_specified}
        COMMAND $<TARGET_FILE:tsepepe_${name}>)
    set_tests_properties(
        ${test_help_printed_test_on_wrong_args_specified} PROPERTIES PASS_REGULAR_EXPRESSION "${help_regex}")

    set(test_return_code_non_zero_for_wrong_args tsepepe_test_${name}_non_zero_return_code_on_wrong_args)
    add_test(
        NAME ${test_return_code_non_zero_for_wrong_args}
        COMMAND $<TARGET_FILE:tsepepe_${name}>)
    set_tests_properties(${test_return_code_non_zero_for_wrong_args} PROPERTIES WILL_FAIL TRUE)

endfunction()

################################################################################
# Main script
################################################################################
find_program(BEHAVE behave REQUIRED)

AddToolTest(function_definition_generator)
AddToolTest(paired_cpp_file_finder)
AddToolTest(abstract_class_finder)
AddToolTest(pure_virtual_functions_extractor)
AddToolTest(suitable_place_in_class_finder)
AddToolTest(full_class_name_expander)

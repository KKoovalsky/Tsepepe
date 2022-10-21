################################################################################
# Functions
################################################################################
function(AddBehaveTest name)

    add_test(
        NAME tsepepe_test_${name}
        COMMAND ${BEHAVE} -D tool_path=$<TARGET_FILE:tsepepe_${name}> ${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

endfunction()

################################################################################
# Main script
################################################################################
find_program(BEHAVE behave REQUIRED)

AddBehaveTest(function_definition_generator)
AddBehaveTest(paired_cpp_file_finder)
AddBehaveTest(abstract_class_finder)
AddBehaveTest(pure_virtual_functions_extractor)

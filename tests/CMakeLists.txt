################################################################################
# Functions
################################################################################
function(AddBehaveTest name)

    add_test(
        NAME tsepepe_test_${name}
        COMMAND ${BEHAVE} -D tool_path=$<TARGET_FILE:tsepepe_${name}>
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${name}
    )

endfunction()

################################################################################
# Main script
################################################################################
find_program(BEHAVE behave REQUIRED)

AddBehaveTest(function_definition_generator)

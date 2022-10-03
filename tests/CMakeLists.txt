find_program(BEHAVE behave REQUIRED)
add_test(
    NAME tsepepe_test_function_definition_generator
    COMMAND ${BEHAVE} -D tool_path=$<TARGET_FILE:tsepepe_definition_generator>
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/function_definition_generator
)

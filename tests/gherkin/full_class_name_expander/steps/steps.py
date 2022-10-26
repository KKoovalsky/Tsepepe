import os
import subprocess
from helpers.file import File
from helpers.tool_result import ToolResult
from hamcrest import assert_that, equal_to, empty
import helpers.utils as utils


@given("Header file with content")
def step_impl(context):
    header = os.path.join(context.working_directory, "header.hpp")
    file_content = context.text
    File(header, file_content).create()


@when('Class name "{class_name}" is expanded')
def step_impl(context, class_name: str):
    header_path = os.path.join(context.working_directory, "header.hpp")
    tool_path = utils.get_tool_path(context)
    comp_db_dir = context.working_directory
    cmd = [tool_path, comp_db_dir, header_path, class_name]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@then('The result is "{expected_stdout}"')
def step_impl(context, expected_stdout: str):
    result = utils.get_result(context)
    stdout = result.stdout
    assert_that(stdout, equal_to(expected_stdout))
    assert_that(result.return_code, equal_to(0))
    assert_that(result.stderr, empty())

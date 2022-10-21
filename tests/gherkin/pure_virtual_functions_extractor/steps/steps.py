import os
import subprocess
from helpers.file import File
from helpers.tool_result import ToolResult
from hamcrest import assert_that, equal_to, empty, not_
import helpers.utils as utils


@given("Header file with content")
def step_impl(context):
    count = context.unnamed_file_count
    context.unnamed_file_count += 1
    fname = "header_{}.hpp".format(count)
    path = os.path.join(context.working_directory, fname)
    file_content = context.text
    file = File(path, file_content)
    file.create()


@when('Pure virtual functions are extracted from class "{class_name}"')
def step_impl(context, class_name: str):
    count = context.unnamed_file_count - 1
    fname = "header_{}.hpp".format(count)
    header_path = os.path.join(context.working_directory, fname)
    tool_path = utils.get_tool_path(context)
    comp_db_dir = context.working_directory
    cmd = [tool_path, comp_db_dir, header_path, class_name]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@then("Stdout contains")
def step_impl(context):
    stdout = utils.get_result(context).stdout.rstrip()
    expected_stdout = context.text
    assert_that(stdout, equal_to(expected_stdout))


@then("No errors are emitted")
def step_impl(context):
    result = utils.get_result(context)
    assert_that(result.return_code, equal_to(0))
    assert_that(result.stderr, empty())

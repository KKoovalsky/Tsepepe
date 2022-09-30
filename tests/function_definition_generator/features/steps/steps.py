import os
import behave
import subprocess
from behave.runner import Context
from hamcrest import assert_that, equal_to, empty
from features.helpers.file import File
from features.helpers.utils import get_tool_path, get_result
from features.helpers.tool_result import ToolResult


@given("Header File With Content")
def step_impl(context: Context):
    count = context.unnamed_file_count
    context.unnamed_file_count += 1
    path = os.path.join(os.getcwd(), "some_header{}.hpp".format(count))
    file_content = context.text
    file = File(path, file_content)
    file.create()
    context.created_files.append(file)


@when("Method Definition Is Generated From Declaration At Line {line}")
def step_impl(context, line):
    if len(context.created_files) == 0:
        raise RuntimeError(
            (
                "No file defined to pass to the tool!"
                'Use "Given" expression to define a header file.'
            )
        )

    tool_path = get_tool_path(context)
    header_file_path = context.created_files[-1].path
    cmd = [tool_path, "-p", context.comp_db.path, header_file_path]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@then("Stdout contains")
def step_impl(context):
    expected_stdout = context.text
    result = get_result(context)
    assert_that(result.stdout.rstrip(), equal_to(expected_stdout))


@then("No errors are emitted")
def step_impl(context):
    result = get_result(context)
    assert_that(result.stderr, empty())
    assert_that(result.return_code, equal_to(0))

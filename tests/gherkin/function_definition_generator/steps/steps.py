import os
import subprocess
from behave.runner import Context
from behave import given, when, then
from hamcrest import assert_that, equal_to, empty, not_
from helpers.file import File
from helpers.utils import get_tool_path, get_result, get_file_content
from helpers.tool_result import ToolResult


@given("Header file with content")
def step_impl(context: Context):
    count = context.unnamed_file_count
    context.unnamed_file_count += 1
    path = os.path.join(os.getcwd(), "some_header{}.hpp".format(count))
    file_content = context.text
    file = File(path, file_content)
    file.create()
    context.created_files.append(file)


@given('Header file called "{filename}" with content')
def step_impl(context: Context, filename: str):
    path = os.path.join(os.getcwd(), filename)
    file_content = context.text
    file = File(path, file_content)
    file.create()
    context.created_files.append(file)


@when("Method definition is generated from declaration at line {line}")
def step_impl(context, line: int):
    if len(context.created_files) == 0:
        raise RuntimeError(
            "No file defined to pass to the tool!"
            'Use "Given" expression to define a header file.'
        )
    tool_path = get_tool_path(context)
    header_file_path = context.created_files[-1].path
    header_file_content = context.created_files[-1].content
    cmd = [
        tool_path,
        context.comp_db.path,
        header_file_path,
        header_file_content,
        line,
    ]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@when(
    "Method definition is generated from declaration "
    'in file "{filename}" at line {line}'
)
def step_impl(context, filename: str, line: int):
    if len(context.created_files) == 0:
        raise RuntimeError(
            "No file defined to pass to the tool!"
            'Use "Given" expression to define a header file.'
        )
    tool_path = get_tool_path(context)
    file_path = os.path.join(os.getcwd(), filename)
    file_content = get_file_content(file_path)
    cmd = [tool_path, context.comp_db.path, file_path, file_content, line]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@then("Stdout contains")
def step_impl(context: Context):
    expected_stdout = context.text + "\n{\n}\n"
    stdout = get_result(context).stdout
    assert_that(stdout, equal_to(expected_stdout))


@then("No errors are emitted")
def step_impl(context):
    result = get_result(context)
    assert_that(result.stderr, empty())
    assert_that(result.return_code, equal_to(0))


@then("Error is raised")
def step_impl(context):
    result = get_result(context)
    assert_that(result.return_code, not_(equal_to(0)))

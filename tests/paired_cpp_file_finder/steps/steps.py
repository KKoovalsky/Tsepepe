import os
import subprocess
from pathlib import Path
from hamcrest import assert_that, equal_to, not_, any_of, starts_with
from helpers.utils import get_tool_path, get_result
from helpers.tool_result import ToolResult


@given("C++ file under path {path}")
def step_impl(context, path: str):
    normalized_path = os.path.normpath(path)
    if os.path.isabs(path):
        raise ValueError("Path must be relative! Got: {}".format(path))
    abs_path = os.path.join(context.working_directory, normalized_path)
    directory = os.path.dirname(abs_path)
    Path(directory).mkdir(parents=True, exist_ok=True)
    open(abs_path, "w")


@when("Paired file for {path} is searched")
def step_impl(context, path: str):
    tool_path = get_tool_path(context)
    project_root_dir = context.working_directory
    cmd = [tool_path, project_root_dir, path]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@then("Finding result is {path}")
def step_impl(context, path: str):
    expected_stdout = os.path.join(context.working_directory, path)
    stdout = get_result(context).stdout.rstrip()
    assert_that(stdout, equal_to(expected_stdout))


@then("Finding results are {path1} and {path2}")
def step_impl(context, path1: str, path2: str):
    full_path1 = os.path.join(context.working_directory, path1)
    full_path2 = os.path.join(context.working_directory, path2)
    expected_stdout1 = "{}\n{}\n".format(full_path1, full_path2)
    expected_stdout2 = "{}\n{}\n".format(full_path2, full_path1)
    stdout = get_result(context).stdout
    assert_that(stdout, any_of(expected_stdout1, expected_stdout2))


@then("No paired file found error is raised")
def step_impl(context):
    result = get_result(context)
    return_code = result.return_code
    stderr = result.stderr
    assert_that(return_code, not_(equal_to(0)))
    assert_that(stderr, starts_with("ERROR: No paired C++ file found"))

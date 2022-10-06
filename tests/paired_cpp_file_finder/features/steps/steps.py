import os
import subprocess
from pathlib import Path
from hamcrest import assert_that, equal_to, empty, not_
from features.helpers.utils import get_tool_path, get_result
from features.helpers.tool_result import ToolResult


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

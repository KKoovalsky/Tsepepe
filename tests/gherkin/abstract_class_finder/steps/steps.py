import os
import subprocess
from hamcrest import assert_that, equal_to, empty, not_
import helpers.utils as utils
from helpers.tool_result import ToolResult


@given('Some dummy abstract class "{class_name}" under path "{path}"')
def step_impl(context, class_name: str, path: str):
    dummy_hppfile_content = (
        "struct {0}\n"
        "{{\n"
        "    virtual bool is_dummy() = 0;\n"
        "    virtual void do_dummy_stuff(int times) = 0;\n"
        "    virtual ~{0} = default;\n"
        "}}\n"
    ).format(class_name)
    full_path = os.path.join(context.working_directory, path)
    utils.create_file(full_path, dummy_hppfile_content)


@given('Some dummy class "{class_name}" under path "{path}"')
def step_impl(context, class_name: str, path: str):
    dummy_hppfile_content = (
        "class {0}\n"
        "{{\n"
        "  public:"
        "    bool is_yolo();\n"
        "    unsigned int get_stuff(float f);\n"
        "}}\n"
    ).format(class_name)
    full_path = os.path.join(context.working_directory, path)
    utils.create_file(full_path, dummy_hppfile_content)


@given('Header file "{path}" with content')
def step_impl(context, path: str):
    content = context.text
    full_path = os.path.join(context.working_directory, path)
    utils.create_file(full_path, content)


@when('Abstract class "{class_name}" is tried to be found')
def step_impl(context, class_name: str):
    tool_path = utils.get_tool_path(context)
    root = context.working_directory
    cmd = [tool_path, root, root, class_name]
    cmd_result = subprocess.run(cmd, capture_output=True)
    context.result = ToolResult(
        cmd_result.stdout, cmd_result.stderr, cmd_result.returncode
    )


@then('Path "{path}" is returned')
def step_impl(context, path):
    stdout = utils.get_result(context).stdout.strip()
    expected_stdout = os.path.join(context.working_directory, path)
    assert_that(stdout, equal_to(expected_stdout))


@then("No error is raised")
def step_impl(context):
    result = utils.get_result(context)
    assert_that(result.return_code, equal_to(0))
    assert_that(result.stderr, empty())


@then("No match is found")
def step_impl(context):
    result = utils.get_result(context)
    assert_that(result.return_code, not_(equal_to(0)))
    assert_that(result.stderr, not_(empty()))

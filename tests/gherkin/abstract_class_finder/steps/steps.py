import os
import helpers.utils as utils


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
    raise NotImplementedError(
        'STEP: When Abstract class "TheClass" is tried to be found'
    )


@then('Path "{path}" is returned')
def step_impl(context):
    raise NotImplementedError(
        'STEP: Then Path "some/dir2/the_class.hpp" is returned'
    )

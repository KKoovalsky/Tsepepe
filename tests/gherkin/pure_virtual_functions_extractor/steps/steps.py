import os
from helpers.file import File


@given("Header file with content")
def step_impl(context):
    count = context.unnamed_file_count
    context.unnamed_file_count += 1
    fname = "header_{}.hpp".format(count)
    path = os.path.join(context.working_directory, fname)
    file_content = context.text
    file = File(path, file_content)
    file.create()


@when("Pure virtual functions are extracted")
def step_impl(context):
    raise NotImplementedError("STEP: When Pure virtual functions are extracted")


@then("Stdout contains")
def step_impl(context):
    raise NotImplementedError("STEP: Then Stdout contains")


@then("No errors are emitted")
def step_impl(context):
    raise NotImplementedError("STEP: Then No errors are emitted")

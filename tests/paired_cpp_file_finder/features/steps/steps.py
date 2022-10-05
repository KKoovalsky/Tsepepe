import os
from pathlib import Path


@given("C++ file under path {path}")
def step_impl(context, path: str):
    normalized_path = os.path.normpath(path)
    if os.path.isabs(path):
        raise ValueError("Path must be relative! Got: {}".format(path))
    abs_path = os.path.join(context.working_directory, normalized_path)
    directory = os.path.dirname(abs_path)
    Path(directory).mkdir(parents=True, exist_ok=True)
    open(abs_path, 'w')


@when("Paired file for {path} is searched")
def step_impl(context, path: str):
    raise NotImplementedError("STEP: When Paired file for dir/file1.hpp is searched")


@then("Finding result is {path}")
def step_impl(context, path: str):
    raise NotImplementedError("STEP: Then Finding result is dir/file1.cpp")

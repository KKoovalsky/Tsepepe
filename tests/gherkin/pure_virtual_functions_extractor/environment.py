import os
import shutil
from helpers.compilation_database import CompilationDatabase


def before_scenario(context, scenario):
    context.unnamed_file_count = 0
    context.working_directory = os.path.join(os.getcwd(), "temp")
    os.mkdir(context.working_directory)
    CompilationDatabase(context.working_directory).create()


def after_scenario(context, scenario):
    shutil.rmtree(context.working_directory)

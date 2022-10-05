import os
import shutil


def before_scenario(context, scenario):
    context.working_directory = os.path.join(os.getcwd(), "temp")
    os.mkdir(context.working_directory)


def after_scenario(context, scenario):
    shutil.rmtree(context.working_directory)

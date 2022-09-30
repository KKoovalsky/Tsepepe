import os
from features.helpers.compilation_database import CompilationDatabase


def before_all(context):
    comp_db = CompilationDatabase(os.getcwd())
    comp_db.create()
    context.comp_db = comp_db


def after_all(context):
    context.comp_db.remove()


def before_scenario(context, scenario):
    context.unnamed_file_count = 0
    context.created_files = list()


def after_scenario(context, scenario):
    for f in context.created_files:
        f.remove()

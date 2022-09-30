import os
import subprocess


class CompilationDatabase:
    def __init__(self, path_to_directory_with_compile_db: str):
        self.path = path_to_directory_with_compile_db
        self._comp_db_path = os.path.join(self.path, "compile_commands.json")

    def create(self):
        this_file_path = os.path.dirname(os.path.realpath(__file__))
        dummy_main_path = os.path.join(this_file_path, "misc", "dummy_main.cpp")
        cmd = [
            "bear",
            "--output",
            self._comp_db_path,
            "--",
            "g++",
            dummy_main_path,
            "-o",
            "dummy",
        ]
        subprocess.run(cmd)
        executable_path = os.path.join(os.getcwd(), "dummy")
        os.remove(executable_path)

    def remove(self):
        if os.path.exists(self._comp_db_path):
            os.remove(self._comp_db_path)

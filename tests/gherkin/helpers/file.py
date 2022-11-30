import os


class File:
    def __init__(self, full_path: str, content: str):
        self.path = full_path
        self.content = content

    def create(self):
        with open(self.path, "w") as f:
            f.write(self.content)

    def remove(self):
        if os.path.exists(self.path):
            os.remove(self.path)

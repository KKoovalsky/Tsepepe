class ToolResult:
    def __init__(self, stdout: bytes, stderr: bytes, return_code: int):
        self.stdout = stdout.decode('utf-8')
        self.stderr = stderr.decode('utf-8')
        self.return_code = return_code

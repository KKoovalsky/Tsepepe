from behave.runner import Context


def get_tool_path(context: Context):
    tool_path = context.config.userdata.get("tool_path")
    if not tool_path:
        raise ValueError(
            (
                "No path to the tool under test specified. "
                'Pass it with "behave -Dtool_path=<path/to/the/tool>"'
            )
        )
    return tool_path


def get_result(context: Context):
    if not hasattr(context, "result"):
        raise RuntimeError(
            (
                "The tool under test has not been invoked! "
                'Are you missing a "When" step?'
            )
        )
    return context.result

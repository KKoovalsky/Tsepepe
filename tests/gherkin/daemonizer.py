#!/usr/bin/env python3


import os
import click
import subprocess
from daemonocle import Daemon


@click.command()
@click.argument("name", type=str)
@click.argument("pid-file", type=click.Path())
@click.argument("action", type=click.Choice(["start", "stop"]))
@click.argument("command", type=str, default="")
def daemonizer(name, pid_file, action, command: str):
    """Daemonizes a blocking command making the shell free to accept commands.

    Consider a Server which blocks the current shell when started: typical
    example - an endless loop:

        while True ; do sleep 1 ; done

    If we want to run that program in the background, leaving the current shell
    session still interactive, we have to daemonize that program.

    daemonize.py programs performs exactly that. It creates an UNIX daemon with
    NAME, that is traceable using the PID_FILE. It can be started or stopped
    via the ACTION parameter. The COMMAND is the program to be daemonized.

    NOTE: To run a command with parameters, remember to use surrounding '
    (ticks):

    \b
        ./daemonizer.py SomeCoolName pid.pid start \\
            'while True ; do sleep 1 ; done'

    To stop the daemon:

        ./daemonizer.py SomeCoolName pid.pid stop

    """
    if action == "start" and len(command) == 0:
        raise ValueError("Command cannot be empty when starting the daemon!")

    cmd_runner = CommandRunner(command)
    current_dir = os.getcwd()
    daemon = Daemon(
        name=name, pid_file=pid_file, worker=cmd_runner, work_dir=current_dir
    )
    daemon.do_action(action)


class CommandRunner:
    def __init__(self, command: str):
        self._command = command

    def __call__(self):
        subprocess.run(self._command, shell=True)


if __name__ == "__main__":
    daemonizer()

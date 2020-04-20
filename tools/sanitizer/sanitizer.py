#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
from actions import actionsList

from lib.compilation_db import CompilationDB
from lib.configuration import Configuration

if __name__ == "__main__":

    # Handle arguments
    parser = argparse.ArgumentParser(description="Generate compilation database")
    parser.add_argument("--workspace", "-w", dest="workspace", type=str, required=True, help="Full path of the workspace to be sanitized.")
    parser.add_argument("--output", "-o", dest="output", type=str, default="compile_commands.json")
    parser.add_argument("--config", "-c", dest="config", type=str, default=None, help="Bazel configuration.")
    parser.add_argument("--clang-format", dest="clang_format", default="clang-format")
    parser.add_argument("--eslint", default="eslint")
    parser.add_argument("--action", "-a", dest="actions", action="append", choices=actionsList.keys(), default=[], help="Action(s) to be performed, valid actions are: {}".format(", ".join(actionsList.keys())))
    parser.add_argument(dest="target", nargs="?", type=str, default="//...", help="Target to be used to generate the compilation database.")

    args = parser.parse_args()

    # Generate the configuration object
    config = Configuration(args.workspace, args.config, {
        "clang-format": {"cmd": [args.clang_format], "cmdAssert": [args.clang_format, "--version"]},
        "eslint": {"cmd": [args.eslint], "cmdAssert": [args.eslint, "--version"]}
    })

    # Build the compilation DB
    compilationDB = CompilationDB(config, args.target, args.output)

    print("Performing %i action(s): %s" % (len(args.actions), ", ".join(args.actions)))
    for actionId in args.actions:
        actionsList[actionId](config, compilationDB)

#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os

from lib.utility import assertCommand, executeMultiCommand

def clangFormat(config, compilationDB):

    assertCommand(["clang-format-6.0", "--version"])

    configs = []
    for command in compilationDB.getData():
        configs.append({
            "cmd": ["clang-format-6.0", "-i", "-sort-includes", os.path.relpath(command["file"], config["execRoot"])],
            "cwd": config["workspace"]
        })
    executeMultiCommand(configs)

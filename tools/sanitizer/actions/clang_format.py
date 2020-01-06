#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os

from lib.utility import assertCommand, executeMultiCommand

def clangFormat(config, compilationDB):

    configs = []
    for command in compilationDB.getData():
        configs.append({
            "cmd": config.getToolCmd("clang-format") + ["-style=file", "-i", "-sort-includes", os.path.relpath(command["file"], config.getExecRoot())],
            "cwd": config.getWorkspace()
        })
    executeMultiCommand(configs)

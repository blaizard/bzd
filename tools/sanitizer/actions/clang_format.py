#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os

from lib.utility import assertCommand, executeMultiCommand, executeCommand

def clangFormat(config, compilationDB):

    assertCommand([config.getTool("clang-format"), "--version"])

    configs = []
    for command in compilationDB.getData():
        configs.append({
            "cmd": [config.getTool("clang-format"), "-style=file", "-i", "-sort-includes", os.path.relpath(command["file"], config.getExecRoot())],
            "cwd": config.getWorkspace()
        })
    executeMultiCommand(configs)

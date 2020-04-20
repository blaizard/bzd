#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os

from lib.utility import executeCommand

def eslint(config, compilationDB):

    executeCommand(
        cmd = config.getToolCmd("eslint") + ["--version"],
        cwd = config.getWorkspace()
    )

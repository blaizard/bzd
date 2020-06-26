#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os
import re
import json

from lib.utility import executeCommand

class CompilationDB():

    def __init__(self, config, target, output):

        # Generate the list of queries associated with this target
        queries = executeCommand(["./tools/bazel",
            "query",
            "--noshow_progress",
            "--noshow_loading_progress",
            "kind(\"cc_(library|binary|test|inc_library|proto_library)\", %s)" % (target)
        ], config.getWorkspace()).splitlines()

        # Generate the associated compilation database for each individual target
        print("Generating C++ compilation database for %i target(s), this may take a while..." % (len(queries)))

        # Remove all previous *compile_commands.json if any
        for root, dirs, files in os.walk(config.getBazelBin(), topdown=True):
            for fileName in files:
                if fileName.endswith("compile_commands.json"):
                    os.remove(os.path.join(root, fileName))

        executeCommand(["./tools/bazel",
            "build",
            "--symlink_prefix=/",
            "--aspects=@bazel-compilation-database//:aspects.bzl%compilation_database_aspect",
            "--noshow_progress",
            "--noshow_loading_progress",
            "--output_groups=compdb_files"
        ] + config.getBazelExtraArgs() + queries, config.getWorkspace())

        # Combine all json files together
        compileCommands = []
        for root, dirs, files in os.walk(config.getBazelBin(), topdown=True):
            dirs[:] = [dirName for dirName in dirs if config.isValidPath(os.path.relpath(os.path.join(root, dirName), config.getBazelBin()))]
            for fileName in files:
                if fileName.endswith("compile_commands.json"):
                    with open(os.path.join(root, fileName)) as fd:
                        for line in fd:
                            bc_cc = json.loads(line.strip("\r\n,"))
                            compileCommands.append(bc_cc)

        self.data = self._sanitizeAndFilterCompileCommands(compileCommands, config)
        with open(output, "w") as f:
            json.dump(self.data, f, sort_keys=True, indent=4, separators=(',', ': '))

        self.outputPath = os.path.realpath(output)
        print("Compilation database written to {}".format(self.outputPath))

    """
    Return the content of the compilation DB
    """
    def getData(self):
        return self.data

    """
    Reads all lines from the compile commands and returns a list of cleaned up compile commands
    """
    def _sanitizeAndFilterCompileCommands(self, compileCommands, config):
        result = []
        for command in compileCommands:
            # Exclude files that are not part of the working directory
            if config.isValidPath(command["file"]):
                nl = dict(command)
                nl["directory"] = config.getExecRoot()
                newFile = os.path.join(config.getExecRoot(), command["file"])
                nl["command"] = command["command"].replace(command["file"], newFile)
                nl["command"] += " -Wno-pragma-once-outside-header"  # fixes check for pragma once (clang-tidy bug)
                nl["command"] += " -Wno-unused-variable"  # fixes unused variable, e.g. during header processing (clang-tidy bug)
                nl["file"] = newFile
                result.append(nl)
        return result

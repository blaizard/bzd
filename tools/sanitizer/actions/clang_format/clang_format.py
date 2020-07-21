#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import sys
from bzd.utils.run import localBazelBinary
from tools.sanitizer.utils.workspace import Files

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Wrapper for clang-format")
    parser.add_argument("--clang-format", dest="clang_format",
                        default="external/linux_x86_64_clang_9_0_0/bin/clang-format")
    parser.add_argument("workspace", help="Workspace to be processed.")

    args = parser.parse_args()

    files = Files(args.workspace, include=[
        "**/*.c",
        "**/*.cxx",
        "**/*.cpp",
        "**/*.h",
        "**/*.hpp"
    ])
    noError = True
    for path in files.data():
        result = localBazelBinary(args.clang_format, args=[
                                  "-style=file", "-i", "-sort-includes", path])
        noError = noError and (result.getReturnCode() == 0)

    sys.exit(0 if noError else 1)

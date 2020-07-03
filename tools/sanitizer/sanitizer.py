#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import os
import sys

from bzd.utils.run import localBazelBinary 

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Generate compilation database")
    parser.add_argument("--workspace", dest="workspace", type=str, default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", "."), help="Full path of the workspace to be sanitized.")
    parser.add_argument("--action", dest="actions", action="append", default=[], help="Action(s) to be performed")

    args = parser.parse_args()

    print("Running sanitizer in '{}'...".format(args.workspace))
    noError = True
    for action in args.actions:
        print("Action '{}'...".format(action))
        result = localBazelBinary(action, ignoreFailure=True, args=[args.workspace], timeoutS = 300)
        noError = noError and (result.returncode == 0)
        if result.returncode != 0:
            print(result.getOutput())

    # Return the error code
    sys.exit(0 if noError else 1)

#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import sys
from bzd.utils.run import localBazelBinary

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Wrapper for buildifier")
    parser.add_argument("--buildifier", dest="buildifier",
                        default="external/buildifier/file/downloaded")
    parser.add_argument("workspace", help="Workspace to be processed.")

    args = parser.parse_args()

    result = localBazelBinary(args.buildifier, args=[
                              "-lint", "fix", "-r", args.workspace])
    print(result.getOutput())

    sys.exit(result.getReturnCode())

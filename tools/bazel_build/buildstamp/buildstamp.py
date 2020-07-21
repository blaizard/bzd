#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import json
import platform
from tools.bazel_build.buildstamp.buildstamp_data import BUILDSTAMP

if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Convert a stamp file to json output.")
    parser.add_argument("output", help="Json output.")

    args = parser.parse_args()
    output = {
        "host": platform.platform()
    }
    for key, value in BUILDSTAMP.items():
        if key.startswith("BZD_"):
            output[key[4:].lower()] = value

    with open(args.output, "w") as f:
        f.write(json.dumps(output))

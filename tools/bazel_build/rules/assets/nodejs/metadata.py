#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import io
import os
import re
import shlex
import json
import sys
from typing import Any, Dict


def parseYarn(yarnLockPath: str, packages: Dict[str, Any] = {}) -> Dict[str, Any]:

    pattern = re.compile("^(.*)@([^@]*)[:,]$")

    current = None
    with io.open(yarnLockPath, mode="r", encoding="utf-8") as f:
        line = f.readline()
        while line:
            split = shlex.split(line)
            if len(split) > 0:
                m = pattern.match(split[-1])
                if m:
                    isMatch = True
                    current = None
                    if m.group(1) in packages:
                        current = m.group(1)
                elif current and len(split) == 2:
                    if split[0] == "version":
                        packages[current]["version"] = split[1]
            line = f.readline()

    return packages


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Information generator for NodeJs rules.")
    parser.add_argument("--package_json", type=str,
                        default="packages.json", help="Path of the packages json file.")
    parser.add_argument("--yarn_lock", type=str,
                        help="Path of the packages json file.")
    parser.add_argument("output", default="package.manifest",
                        help="Path of the generated file.")

    args = parser.parse_args()

    with open(args.package_json) as f:
        packagesJson = json.load(f)
    packages = {name: {"requested": version}
                for name, version in packagesJson.get("dependencies", {}).items()}

    output = {}
    if args.yarn_lock:
        output["packages"] = parseYarn(args.yarn_lock, packages)

    else:
        print("No lock file provided.")
        sys.exit(1)

    with open(args.output, "w") as f:
        f.write(json.dumps(output))

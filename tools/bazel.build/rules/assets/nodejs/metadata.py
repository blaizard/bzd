#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import io
import os
import re
import shlex
import json
import sys

def parseYarn(packageNames, yarnLockPath):

	pattern = re.compile("^(.*)@([^@]*)[:,]$")

	packages = {}
	current = None
	with io.open(yarnLockPath, mode = "r", encoding = "utf-8") as f:
		line = f.readline()
		while line:
			split = shlex.split(line)
			if len(split) > 0:
				m = pattern.match(split[-1])
				if m:
					isMatch = True
					current = None
					if m.group(1) in packageNames:
						current = m.group(1)
						packages[current] = {}
				elif current and len(split) == 2:
					if split[0] == "version":
						packages[current]["version"] = split[1]
			line = f.readline()

	return packages

if __name__== "__main__":

	parser = argparse.ArgumentParser(description = "Information generator for NodeJs rules.")
	parser.add_argument("--package_json", type = str, default = "packages.json", help = "Path of the packages json file.")
	parser.add_argument("--yarn_lock", type = str, help = "Path of the packages json file.")
	parser.add_argument("output", default = "package.manifest", help = "Path of the generated file.")

	args = parser.parse_args()

	with open(args.package_json) as f:
		packages = json.load(f)
	packageNames = set(packages.get("dependencies", {}).keys())

	output = {}
	if args.yarn_lock:
		output["packages"] = parseYarn(packageNames, args.yarn_lock)

	else:
		print("No lock file provided.")
		sys.exit(1)

	with open(args.output, "w") as f:
		f.write(json.dumps(output))

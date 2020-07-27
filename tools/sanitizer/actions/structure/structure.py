#!/usr/bin/python

import argparse
import re
import sys
from pathlib import Path
from tools.sanitizer.utils.python.workspace import Files

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Tool enforcing correct direct and file structure")
	parser.add_argument("--regexpr",
		default="^[a-z0-9_/.]+(\.S)?$",
		help="Regular expression to be used for the matching.")
	parser.add_argument("workspace", type=Path, help="Workspace to be processed.")

	args = parser.parse_args()

	files = Files(args.workspace,
		exclude=["**BUILD", "**BUILD.template", "**WORKSPACE", "**README.md", "**Jenkinsfile", "**Dockerfile"])
	regexpr = re.compile(args.regexpr)
	noMathList = []
	for path in files.data(relative=True):
		if not re.match(regexpr, path.as_posix()):
			noMathList.append(path)

	if len(noMathList) > 0:
		print("The following path(s) do not match the naming convention '{}':".format(args.regexpr))
		for path in noMathList:
			print(" - {}".format(path.as_posix()))
		sys.exit(1)

	sys.exit(0)

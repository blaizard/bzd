#!/usr/bin/python

import argparse
import re
import sys
from tools.sanitizer.utils.workspace import Files

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Tool enforcing correct direct and file structure")
	parser.add_argument("--regexpr",
		default="^[a-z0-9_/.]+(\.S)?$",
		help="Regular expression to be used for the matching.")
	parser.add_argument("workspace", help="Workspace to be processed.")

	args = parser.parse_args()

	files = Files(args.workspace,
		exclude=["**BUILD", "**BUILD.template", "**WORKSPACE", "**README.md", "**Jenkinsfile", "**Dockerfile"])
	regexpr = re.compile(args.regexpr)
	noMathList = []
	for path in files.data(relative=True):
		if not re.match(regexpr, path):
			noMathList.append(path)

	if len(noMathList) > 0:
		print("The following path(s) do not match the naming convention '{}':".format(args.regexpr))
		for path in noMathList:
			print(" - {}".format(path))
		sys.exit(1)

	sys.exit(0)

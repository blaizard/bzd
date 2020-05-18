#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import json

if __name__== "__main__":

	parser = argparse.ArgumentParser(description = "Information generator rules.")
	parser.add_argument("-i", "--input", nargs=2, metavar=("app", "path"), action="append", help = "Pair of application name and an information package json file, can add multiple.")
	parser.add_argument("output", default = "package.manifest", help = "Path of the generated file.")

	args = parser.parse_args()

	# Merge all entries together
	output = {}
	for item in args.input:
		with open(item[1]) as f:
			fragment = json.load(f)
		if item[0] not in output:
			output[item[0]] = {}
		output[item[0]].update(fragment)

	with open(args.output, "w") as f:
		f.write(json.dumps(output))

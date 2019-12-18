#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import argparse
import json5

if __name__== "__main__":

	parser = argparse.ArgumentParser(description="Architect configuration file parser")
	parser.add_argument("inputs", nargs="+", help="Input files to be passed to the parser")

	config = parser.parse_args()

	for inputFile in config.inputs:
		print("Loading {}".format(inputFile))
		with open(inputFile) as f:
			manifest = json5.load(f)
		print(manifest)

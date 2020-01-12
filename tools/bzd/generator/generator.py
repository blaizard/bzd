#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import formats
import os
from log import Log
from parser import manifestToDict, dictToManifest
from manifest import Manifest

if __name__== "__main__":

	parser = argparse.ArgumentParser(description="Architect configuration file parser.")
	parser.add_argument("inputs", nargs="+", help="Input files to be passed to the parser.")
	parser.add_argument("-o", "--output", default="./out", type=str, help="Output path of generated file.")
	parser.add_argument("-f", "--format", default="cpp", help="Output format to be used.")
	parser.add_argument("-m", "--manifest", default=None, help="Generate the resulting manifest.")

	config = parser.parse_args()
	assert hasattr(formats, config.format), "Unsupported output format '{}'".format(str(config.format))

	manifest = Manifest()
	for path in config.inputs:
		try:
			data = manifestToDict(path)
			manifest.merge(data)
		except Exception as e:
			Log.fatal("In manifest {}".format(path), e)
	manifest.process()

	# Generate the header comments
	comments = ["This file has been auto-generated and is the result of the following manifest(s):"] + ["- {}".format(path) for path in config.inputs]

	# Generate the output manifest
	if config.manifest:
		with open(config.manifest, "w") as f:
			[f.write("# {}\n".format(comment)) for comment in comments]
			f.write(dictToManifest(manifest.getData()))

	formatter = getattr(formats, config.format)
	formatter(manifest, config.output, {
		"comments": comments
	})
	print(config.output)

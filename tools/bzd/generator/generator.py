#!/usr/bin/python

import argparse
import formats
import re
import os
from log import Log
from parser import manifestToDict, dictToManifest
from manifest import Manifest

if __name__ == "__main__":

	parser = argparse.ArgumentParser(
		description="Architect configuration file parser.")
	parser.add_argument("inputs",
		nargs="*",
		help="Input files to be passed to the parser.")
	parser.add_argument("-o",
		"--output",
		default="./out",
		type=str,
		help="Output path of generated file.")
	parser.add_argument("-f",
		"--format",
		default="cpp",
		help="Output format to be used.")
	parser.add_argument("-m",
		"--manifest",
		default=None,
		help="Generate the resulting manifest.")
	parser.add_argument("-a",
		"--artifact",
		default=[],
		action="append",
		help="Artifacts to be added to the generated code.")

	config = parser.parse_args()
	assert hasattr(formats,
		config.format), "Unsupported output format '{}'".format(
		str(config.format))

	manifest = Manifest()
	for path in config.inputs:
		try:
			data = manifestToDict(path)
			manifest.merge(data, path)
		except Exception as e:
			Log.fatal("{}".format(path), e)

	# Add artifacts if any
	for artifact in config.artifact:
		index = artifact.find(":")
		assert index != -1, "Artifact is malformed: '{}'.".format(artifact)
		manifest.addArtifact(artifact[index + 1:], artifact[0:index])

	try:
		manifest.process()
	except Exception as e:
		Log.fatal("Oups! Something went wrong.", e)

	# Generate the header comments
	comments = [
		"This file has been auto-generated and is the result of the following manifest(s):"
	] + ["- {}".format(path) for path in config.inputs]

	# Generate the output manifest
	if config.manifest:
		with open(config.manifest, "w") as f:
			[f.write("# {}\n".format(comment)) for comment in comments]
			f.write(dictToManifest(manifest.getData()))

	print("Cpp", config.output)
	print("Manifest", config.manifest)

	try:
		formatter = getattr(formats, config.format)
		formatter(manifest, config.output, {"comments": comments})
	except Exception as e:
		Log.fatal("Cannot generate '{}' output".format(config.format), e)

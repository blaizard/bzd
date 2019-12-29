#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import json

def registryBuild(manifest):

	content = "// This file has been auto-generated.\n\n"

	# Include the registry
	content += "#include \"bzd/core/registry.h\"\n\n"

	# Include object dependencies
	interfaces = set([obj.getInterface() for obj in manifest.getObjects()])
	includes = set()
	for interface in interfaces:
		includes.update(interface.getInclude())
	for include in sorted(list(includes)):
		content += "#include \"{}\"\n".format(include)

	return content

def cpp(manifest, output):
	with open(output, "w") as f:

		# Generate registry
		f.write(registryBuild(manifest))

		f.write("#include <iostream>\nint main() { std::cout << \"Hello Wolrd!\" << std::endl; }")

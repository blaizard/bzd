#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

def getUID():
	getUID.uid = getUID.uid if hasattr(getUID, "uid") else -1
	getUID.uid += 1
	return getUID.uid

def paramsToString(obj):
	params = ["\"{}\"".format(param) if isinstance(param, str) else str(param) for param in obj.getParams()]
	return (", " + ", ".join(params)) if len(params) else ""

def registryBuild(manifest):

	content = ""

	# Include bzd dependencies
	content += "#include \"bzd.h\"\n\n"

	# Include object dependencies
	includes = set()
	for interface in manifest.getDependencies():
		includes.update(interface.getIncludes())
	for include in sorted(list(includes)):
		content += "#include \"{}\"\n".format(include)
	content += "\n"

	# Create an empty namespace to ensure that the symbols are not exported
	content += "namespace {\n"

	# Add objects to the registry
	interfaces = set([obj.getInterface() for obj in manifest.getObjects()])
	for interface in interfaces:
		objects = list(manifest.getObjects({"interface": interface.getName()}))
		nbObjects = len(objects)
		content += "// Definition for registry of type '{}'\n".format(interface.getName())
		content += "bzd::declare::Registry<{}, {}> registry{}_;\n".format(interface.getName(), nbObjects, getUID())
		# Add the objects
		for obj in objects:
			# Build the parameters
			params = paramsToString(obj)
			content += "bzd::Registry<{}, {}> object{}_{{\"{}\"{}}};\n".format(obj.getInterface().getName(), obj.getClass(), getUID(), obj.getName(), params)
		content += "\n"

	# Close the empty namespace
	content += "} // namespace"

	return content

def cpp(manifest, output, configuration):
	config = {
		"comments": []
	}
	config.update(configuration)

	with open(output, "w") as f:

		# Write the header comment
		f.write("/**\n")
		[f.write(" * {}\n".format(comment)) for comment in config["comments"]]
		f.write(" */\n")

		# Generate registry
		f.write(registryBuild(manifest))

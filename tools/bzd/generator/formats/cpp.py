#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

def getUID():
	getUID.uid = getUID.uid if hasattr(getUID, "uid") else -1
	getUID.uid += 1
	return getUID.uid

def paramsToString(obj):
	params = ["\"{}\"".format(param) if isinstance(param, str) else repr(param) for param in obj.getParams()]
	return (", " + ", ".join(params)) if len(params) else ""

def registryBuild(manifest):

	manifest.setRenderer({
		"object": "bzd::Registry<{interface}>::get(\"{name}\")"
	})

	content = ""

	# Include bzd dependencies
	content += "#include \"bzd.h\"\n\n"

	# Include object dependencies
	includes = set()
	for interface in manifest.getDependentInterfaces():
		includes.update(interface.getIncludes())
	for include in sorted(list(includes)):
		content += "#include \"{}\"\n".format(include)
	content += "\n"

	# Create an empty namespace to ensure that the symbols are not exported
	content += "namespace {\n\n"

	# Add objects to the registry
	registryList = manifest.getRegistry()

	# Print the registry
	for registry in registryList:
		content += "// Definition for registry of type '{}'\n".format(registry["interface"])
		content += "bzd::Registry<{}>::Declare<{}> registry{}_;\n".format(registry["interface"], len(registry["objects"]), getUID())
		# Add the objects
		for obj in registry["objects"]:
			# Build the parameters
			params = paramsToString(obj)
			content += "bzd::Registry<{}>::Register<{}> object{}_{{\"{}\"{}}};\n".format(obj.getInterface().getName(), obj.getImplementation(), getUID(), obj.getName(), params)
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

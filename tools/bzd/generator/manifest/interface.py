#!/usr/bin/python
"""
Represents an interface
"""


class Interface(object):
	def __init__(self, manifest, identifier):
		self.manifest = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("interfaces",
			{}).get(identifier, {})

	def isValid(self):
		return self.identifier != None

	def getName(self):
		return self.identifier

	def getIncludes(self):
		includes = self.definition.get("includes", [])
		return includes if isinstance(includes, list) else [includes]

	def _getDerivedImplementation(self, name, interfaces):
		if self.manifest.isInterface(name):
			interface = self.manifest.getInterface(name)
			derivedImplementation = interface.getImplementation()
			if derivedImplementation:
				assert derivedImplementation not in interfaces, "There is a loop in the implementation inheritance for this interface: {}".format(
					", ".join(list(interfaces)))
				interfaces.add(derivedImplementation)
				return self._getDerivedImplementation(derivedImplementation,
					interfaces)
		return name

	def getImplementation(self):
		implementation = self.definition.get("implementation", None)
		return self._getDerivedImplementation(implementation,
			set()) if implementation else None

	def getImplementationOrInterface(self):
		implementation = self.getImplementation()
		return implementation if implementation else self.getName()


"""
Helper to ceate an empty interface
"""


class EmptyInterface(Interface):
	def __init__(self, identifier):
		self.manifest = None
		self.identifier = identifier
		self.definition = {}

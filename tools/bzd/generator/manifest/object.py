#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from .validator import ValidatorReference
from .validator import ValidatorObject
from .validator import ValidatorInterface
from .interface import EmptyInterface

"""
Represents an object
"""
class Object():

	def __init__(self, manifest, identifier):
		self.manifest = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("objects", {}).get(identifier, {})
		self.deps = {
			"interface": set([self.getInterfaceName()]),
			"object": {}
		}

		# Process the definitions
		def visit(value):
			if ValidatorReference.isMatch(value):
				valueStr = value.getRepr()
				# If this represents an object
				if ValidatorObject.isMatch(valueStr):
					result = ValidatorObject.parse(valueStr)
					self.deps["interface"].add(result["interface"])
					self.deps["object"][result["interface"]] = self.deps["object"].get(result["interface"], set())
					self.deps["object"][result["interface"]].add(result["name"])
					value.setReprCallback(self.manifest, lambda renderer : renderer.get("object", "").format(interface = result["interface"], name = result["name"]))
				# If this represents an interface
				elif ValidatorInterface.isMatch(valueStr):
					self.deps["interface"].add(valueStr)
		self._walk(self.definition, visit)

	"""
	Return the interface of the object
	"""
	def getInterface(self):
		name = self.getInterfaceName()
		return self.manifest.getInterface(name) if self.manifest.isInterface(name) else EmptyInterface(name)

	"""
	Return the interface of the object
	"""
	def getInterfaceName(self):
		return self.identifier.split(".")[0]

	"""
	Walk through all values of a dictionary and call a callback
	"""
	@staticmethod
	def _walk(obj, callback):
		for value in (obj.values() if isinstance(obj, dict) else obj):
			if isinstance(value, dict):
				Object._walk(value, callback)
			elif isinstance(value, list):
				Object._walk(value, callback)
			else:
				callback(value)

	"""
	Get all dependent interfaces
	"""
	def getDependentInterfaces(self):
		return [self.manifest.getInterface(dependency) for dependency in self.deps["interface"] if self.manifest.isInterface(dependency)]

	"""
	Get all dependent objects
	"""
	def getDependentObjects(self):
		return self.deps["object"]

	"""
	Return the name of the object
	"""
	def getName(self):
		return self.identifier.split(".")[1]

	"""
	Get the constructor class
	"""
	def getClass(self):
		classInterface = self.getInterface().getClass()
		classObjecy = self.definition.get("class", None)
		assert classInterface == None or classObjecy == None, "Both interface class '{}' and object class '{}' cannot be set at the same time for object identifier '{}'.".format(classInterface, classObjecy, self.identifier)
		return classInterface if classInterface else self.definition.get("class", self.getInterfaceName())

	"""
	Get parameters
	"""
	def getParams(self):
		return self.definition.get("params", [])
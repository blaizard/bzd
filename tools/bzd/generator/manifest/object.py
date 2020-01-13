#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from .validator import ValidatorReference

"""
Represents an object
"""
class Object():

	def __init__(self, manifest, identifier):
		self.manifest = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("objects", {}).get(identifier, {})

	"""
	Return the interface of the object
	"""
	def getInterface(self):
		return self.manifest.getInterface(self.getInterfaceName())

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
	Get all dependencies associated with this object
	"""
	def getDependencies(self):

		dependencies = set()

		# Look for any references in the object data
		def visit(value):
			if ValidatorReference.isMatch(value) and self.manifest.isInterface(value):
				dependencies.add(self.manifest.getInterface(value))
		self._walk(self.definition, visit)

		# Depend on the object interface itself
		dependencies.add(self.getInterface())

		return dependencies

	"""
	Return the name of the object
	"""
	def getName(self):
		return self.identifier.split(".")[1]

	"""
	Get the constructor class
	"""
	def getClass(self):
		return self.definition.get("class", self.getInterfaceName())

	"""
	Get parameters
	"""
	def getParams(self):
		return self.definition.get("params", [])
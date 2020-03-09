#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from .object import Object
from .interface import Interface, EmptyInterface
from .artifact import Artifact
from .validator import Validator

"""
Represent a key, used together with the context for messaging
"""
class _Keys():

	def __init__(self):
		self.keys = []

	def push(self, key):
		self.keys.append(key)

	def pop(self):
		self.keys.pop()

	def toKey(self):
		return "/".join(self.keys)

	def __str__(self):
		return " > ".join(self.keys)

class Manifest():

	def __init__(self):
		self.data = {}
		self.objects = {}
		self.interfaces = {}
		self.artifacts = {}
		self.sources = {}
		self.setRenderer()
		self.format = {
			"artifacts": {
				"_default": {
					"_key": Validator("any"),
					"path": Validator("path")
				}
			},
			"interfaces": {
				"_default": {
					"_key": Validator("interface"),
					"includes": Validator("any"),
					"implementation": Validator("interface")
				}
			},
			"objects": {
				"_default": {
					"_key": Validator("object"),
					"implementation": Validator("interface"),
					"params": Validator("any"),
					"config": {
						"_key": Validator("any"),
						"_default": Validator("any")
					}
				}
			}
		}

	"""
	Set a new renderer for reference objects
	"""
	def setRenderer(self, renderer = None):
		self.renderer = renderer

	"""
	List all objects
	"""
	def getObjects(self, filt = {}):
		config = {
			"interface": None
		}
		config.update(filt)
		for identifier, obj in self.objects.items():
			if config["interface"] == None or config["interface"] == obj.getInterfaceName():
				yield obj

	"""
	Return true if this is a known interface, false otherwise
	"""
	def isInterface(self, name):
		return str(name) in self.interfaces

	"""
	Get a specific inteface
	"""
	def getInterface(self, name, mustExists = True):
		exists = self.isInterface(name)
		assert not mustExists or exists, "The interface name '{}' was not discovered or is not valid.".format(str(name))
		return self.interfaces[str(name)] if exists else EmptyInterface(name)

	"""
	Return the raw data of this manifest
	"""
	def getData(self):
		return self.data

	"""
	Return the list of artifact objects
	"""
	def getArtifacts(self):
		return self.artifacts.values()

	"""
	List all depending interfaces
	"""
	def getDependentInterfaces(self):

		# Gather all object dependencies
		dependencies = set()
		for obj in self.getObjects():
			dependencies.update(obj.getDependentInterfaces())

		return dependencies

	"""
	Return the registry sorted by dependency resolution
	"""
	def getRegistry(self):
		registryEntries = {}
		interfaces = set([obj.getInterface() for obj in self.getObjects()])
		for interface in interfaces:
			objects = list(self.getObjects({"interface": interface.getName()}))
			# Generate dictionary of depending objects
			deps = {}
			[deps.update(obj.getDependentObjects()) for obj in objects]
			registryEntries[interface.getName()] = {
				"objects": objects,
				"deps": deps
			}
		# Create a dependency graph
		dependencyGraph = {interface: set() for interface in registryEntries.keys()}
		for interface, data in registryEntries.items():
			for dependency in data["deps"].keys():
				dependencyGraph[interface].add(dependency)

		# Sort the registry
		# Todo, can be optimized/simplified
		registryList = []
		while dependencyGraph:
			newDependencyGraph = {}
			registryListInitialLen = len(registryList)
			for interface, data in dependencyGraph.items():
				if all([dep not in dependencyGraph for dep in data]):
					registryList.append({"interface": interface, "objects": registryEntries[interface]["objects"]})
				else:
					newDependencyGraph[interface] = data
			if registryListInitialLen == len(registryList):
				raise Exception("Circular depedency detected between interfaces: {}.".format(", ".join(dependencyGraph.keys())))
			dependencyGraph = newDependencyGraph

		return registryList

	"""
	Merge data into the current manifest.
	"""
	def merge(self, data, path):
		context = {"key": _Keys()}
		try:
			self._mergeAndValidate(self.data, data, self.format, context, path)
		except Exception as e:
			raise Exception("Error while merging ({}): {}".format("; ".join(["{}: '{}'".format(str(key), str(text)) for key, text in context.items() if text]), e))

	"""
	Add artifacts to the generated code.
	"""
	def addArtifact(self, path, identifier):
		if "artifacts" not in self.data:
			self.data["artifacts"] = {}
		assert identifier not in self.data["artifacts"], "Identifier '{}' is already used for artifact.".format(identifier)
		self.data["artifacts"][identifier] = {
			"path": path
		}

	"""
	Process the current data of the manifest in order to speed-up later accesses
	"""
	def process(self):

		# Objects
		self.objects = {}
		for identifier in self.data.get("objects", {}).keys():
			try:
				self.objects[identifier] = Object(self, identifier)
			except Exception as e:
				raise Exception("Error while processing object {}: {}".format(identifier, e))

		# Interfaces
		self.interfaces = {}
		for identifier in self.data.get("interfaces", {}).keys():
			try:
				self.interfaces[identifier] = Interface(self, identifier)
			except Exception as e:
				raise Exception("Error while processing interface {}: {}".format(identifier, e))
	
		# Artifacts
		self.artifacts = {}
		for identifier in self.data.get("artifacts", {}).keys():
			try:
				self.artifacts[identifier] = Artifact(self, identifier)
				self.artifacts[identifier].registerObject(self.objects)
			except Exception as e:
				raise Exception("Error while processing artifact {}: {}".format(identifier, e))

	"""
	Merge the data
	"""
	@staticmethod
	def _mergeAndValidate(dst, src, validation, context, path):

		for key, value in src.items():
			context["key"].push(key)

			# Look for the validation data associated with this key
			vData = validation[key] if key in validation else (validation["_default"] if "_default" in validation else None)
			assert vData != None, "Invalid key '{}'.".format(key)

			# Validate the data
			if isinstance(vData, dict):
				assert isinstance(value, dict), "Key '{}' must contain a dictionary type object.".format(key)

				# If it contains key format validator
				if "_key" in vData:
					vData["_key"].validate(key)

				if key not in dst:
					dst[key] = {}

				# Go further
				Manifest._mergeAndValidate(dst[key], value, vData, context, path)

			else:
				vData.validate(value)
				if key not in dst:
					dst[key] = value
				else:
					assert dst[key] == value, "Conflicting values: '{}' != '{}'".format(str(dst[key]), str(value))

			context["key"].pop()

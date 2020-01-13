#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from .object import Object
from .interface import Interface
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

	def __str__(self):
		return " > ".join(self.keys)

class Manifest():

	def __init__(self):
		self.data = {}
		self.objects = {}
		self.interfaces = {}
		self.format = {
			"interfaces": {
				"_default": {
					"_key": Validator("interface"),
					"includes": Validator("any")
				}
			},
			"objects": {
				"_default": {
					"_key": Validator("object"),
					"_default": Validator("any"),
					"class": Validator("interface"),
					"params": Validator("any")
				}
			}
		}

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
	def getInterface(self, name):
		assert self.isInterface(name), "The interface name '{}' was not discovered or is not valid.".format(str(name))
		return self.interfaces[str(name)]

	"""
	Return the raw data of this manifest
	"""
	def getData(self):
		return self.data

	"""
	List all depending interfaces
	"""
	def getDependencies(self):

		# Gather all object dependencies
		dependencies = set()
		for obj in self.getObjects():
			dependencies.update(obj.getDependencies())

		return dependencies
			
	"""
	Merge data into the current manifest.
	"""
	def merge(self, data):
		context = {"key": _Keys()}
		try:
			self._mergeAndValidate(self.data, data, self.format, context)
		except Exception as e:
			raise Exception("Error while merging ({}): {}".format("; ".join(["{}: '{}'".format(str(key), str(text)) for key, text in context.items() if text]), e))

	"""
	Process the current data of the manifest in order to speed-up later accesses
	"""
	def process(self):
		self.objects = {identifier: Object(self, identifier) for identifier in self.data.get("objects", {}).keys()}
		self.interfaces = {identifier: Interface(self, identifier) for identifier in self.data.get("interfaces", {}).keys()}
		self.interfaces.update({obj.getInterfaceName(): Interface(self, obj.getInterfaceName()) for obj in self.objects.values()})

	"""
	Merge the data
	"""
	@staticmethod
	def _mergeAndValidate(dst, src, validation, context):

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
				Manifest._mergeAndValidate(dst[key], value, vData, context)

			else:
				vData.validate(value)
				if key not in dst:
					dst[key] = value
				else:
					assert dst[key] == value, "Conflicting values: '{}' != '{}'".format(str(dst[key]), str(value))

			context["key"].pop()

#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

from log import Log

"""
Validate a string
"""
class FormatValidator():
	
	def __init__(self, *formats):
		self.formats = list(formats)

	def validate(self, string):
		return True

"""
Represent a key, used together with the context for messaging
"""
class Keys():

	def __init__(self):
		self.keys = []

	def push(self, key):
		self.keys.append(key)

	def pop(self):
		self.keys.pop()

	def __str__(self):
		return " > ".join(self.keys)

"""
Represents an interface
"""
class Interface():

	def __init__(self, manifest, definition):
		self.manifest = manifest
		self.definition = definition

	def getInclude(self):
		include = self.definition.get("include", [])
		return include if isinstance(include, list) else [include]

"""
Represents an object
"""
class Object():

	def __init__(self, manifest, identifier):
		self.manifest = manifest
		self.identifier = identifier

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
	Return the name of the object
	"""
	def getName(self):
		return self.identifier.split(".")[1]

class Manifest():

	def __init__(self):
		self.data = {}
		self.format = {
			"interfaces": {
				"_default": {
					"_key": FormatValidator("class"),
					"include": FormatValidator("path")
				}
			},
			"objects": {
				"_default": {
					"_key": FormatValidator("class"),
					"_default": FormatValidator("any"),
					"class": FormatValidator("class")
				}
			}
		}

	"""
	List all objects
	"""
	def getObjects(self):
		for identifier in self.data.get("objects", {}).keys():
			yield Object(self, identifier)

	"""
	Get a specific inteface
	"""
	def getInterface(self, name):
		return Interface(self, self.data.get("interfaces", {}).get(name, {}))

	"""
	Return the raw data of this manifest
	"""
	def getData(self):
		return self.data

	"""
	Merge data into the current manifest.
	"""
	def merge(self, data, context):
		try:
			context["key"] = Keys()
			self._mergeAndValidate(self.data, data, self.format, context)
		except Exception as e:
			Log.fatal("Error while merging ({})".format("; ".join(["{}: '{}'".format(str(key), str(text)) for key, text in context.items() if text])), e)

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

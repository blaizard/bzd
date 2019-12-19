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

class Manifest():

	def __init__(self):
		self.data = {}
		self.format = {
			"types": {
				"*": {
					"_key": FormatValidator("class"),
					"include": FormatValidator("path")
				}
			},
			"objects": {
				"*": {
					"_key": FormatValidator("class"),
					"class": FormatValidator("class"),
					"*": FormatValidator("any")
				}
			}
		}

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
			vData = validation[key] if key in validation else (validation["*"] if "*" in validation else None)
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

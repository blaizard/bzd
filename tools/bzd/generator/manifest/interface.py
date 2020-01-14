#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

"""
Represents an interface
"""
class Interface(object):

	def __init__(self, manifest, identifier):
		self.manifest = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("interfaces", {}).get(identifier, {})

	def isValid(self):
		return self.identifier != None

	def getName(self):
		return self.identifier

	def getIncludes(self):
		includes = self.definition.get("includes", [])
		return includes if isinstance(includes, list) else [includes]

"""
Helper to ceate an empty interface
"""
class EmptyInterface(Interface):
	def __init__(self, identifier):
		self.manifest = None
		self.identifier = identifier
		self.definition = {}

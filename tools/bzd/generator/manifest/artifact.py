#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os
from .object import Object

class DummyType(object):
	def __init__(self, value):
		self.value = value
	
	def __repr__(self):
		return self.value

"""
Represents an artifact
"""
class Artifact(object):

	uid = 0

	def __init__(self, manifest, identifier):
		self.manifest = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("artifacts", {}).get(identifier, {})
		self.uid = Artifact.uid
		Artifact.uid += 1

	def getName(self):
		return self.identifier

	def getUid(self):
		return self.uid

	def getPath(self):
		path = self.definition.get("path")
		assert path, "Path for artifact '{}' cannot be empty.".format(self.identifier)
		return path

	def registerObject(self, registry):
		identifier = "bzd::Artifact.{}".format(self.identifier)
		assert identifier not in registry, "Object '{}' already registered.".format(identifier)
		registry[identifier] = Object(self.manifest, identifier, {
			"params": [
				DummyType("artifact{}_.data()".format(self.getUid())),
				DummyType("artifact{}_.size()".format(self.getUid()))
			]
		})

	"""
	Open the artifact and return a file stream
	"""
	def read(self):
		path = self.getPath()
		assert os.path.isfile(path), "There is no valid file at '{}'.".format(path)
		with open(path, "rb") as f:
			byte = f.read(1)
			while byte:
				yield byte.decode("ascii")
				byte = f.read(1)

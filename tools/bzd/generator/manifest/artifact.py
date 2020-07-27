#!/usr/bin/python

import os
from .object import Object
from .custom import Custom

from typing import Iterable, MutableMapping, TYPE_CHECKING
if TYPE_CHECKING:
	from .manifest import Manifest
"""
Represents an artifact
"""


class Artifact(object):

	def __init__(self, manifest: "Manifest", identifier: str) -> None:
		self.manifest = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("artifacts", {}).get(identifier, {})

	def getName(self) -> str:
		return self.identifier

	def getPath(self) -> str:
		path = self.definition.get("path")
		assert path, "Path for artifact '{}' cannot be empty.".format(self.identifier)
		assert isinstance(path, str), "Path for artifact '{}' must be a string.".format(self.identifier)
		return str(path)

	def registerObject(self, registry: MutableMapping[str, Object]) -> None:
		identifier = "bzd::Artifact.{}".format(self.identifier)
		assert identifier not in registry, "Object '{}' already registered.".format(identifier)
		registry[identifier] = Object(self.manifest, identifier, {"params": [Custom("artifact", self)]})

	"""
	Open the artifact and return a file stream
	"""

	def read(self) -> Iterable[str]:
		path = self.getPath()
		assert os.path.isfile(path), "There is no valid file at '{}'.".format(path)
		with open(path, "rb") as f:
			byte = f.read(1)
			while byte:
				yield byte.decode("ascii")
				byte = f.read(1)

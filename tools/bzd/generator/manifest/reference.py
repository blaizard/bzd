#!/usr/bin/python

from typing import Callable, Any, Optional, TYPE_CHECKING
if TYPE_CHECKING:
	from .manifest import Manifest
"""
Represents a reference value
"""


class Reference(object):
	__bzd_reference__ = True

	def __init__(self) -> None:
		self.reprCallback: Optional[Callable[[Any], str]] = None
		self.manifest: Optional["Manifest"] = None
		self.value = None

	def getRepr(self) -> str:
		if self.manifest and self.reprCallback and self.manifest.renderer:
			return self.reprCallback(self.manifest.renderer)
		return str(self.value)

	def setReprCallback(self, manifest: "Manifest", callback: Callable[[Any], str]) -> None:
		assert self.reprCallback == None, "Repr callback has already been set for this value: {}.".format(
			str(self.value))
		self.reprCallback = callback
		self.manifest = manifest

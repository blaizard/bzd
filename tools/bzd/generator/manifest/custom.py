from typing import Any, Callable, Optional, TYPE_CHECKING

if TYPE_CHECKING:
	from tools.bzd.generator.manifest.manifest import Manifest
"""
Represents a custom value
"""

ReprCallback = Callable[[Any], Callable[..., str]]


class Custom(object):
	__bzd_custom__ = True

	def __init__(self, customType: str, value: Any, *args: Any) -> None:
		self.type = customType
		self.value = value
		self.args = args
		self.reprCallback: Optional[ReprCallback] = None
		self.manifest: Optional["Manifest"] = None

	def __repr__(self) -> str:
		if self.reprCallback and self.manifest and self.manifest.renderer:
			callback = self.reprCallback(self.manifest.renderer)
			if callback:
				return callback(self.value, *self.args)
		return str(self.value)

	def setReprCallback(self, manifest: "Manifest", callback: ReprCallback) -> None:
		assert self.reprCallback == None, "Repr callback has already been set for this value: {}.".format(
			str(self.value))
		self.reprCallback = callback
		self.manifest = manifest

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error


class Entity:

	def __init__(self, element: Element) -> None:
		self.element = element

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name")

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve the current symbol.
		"""
		pass

	def error(self, message: str) -> None:
		Error.handleFromElement(element=self.element, message=message)

	def assertTrue(self, condition: bool, message: str) -> None:
		if not condition:
			self.error(message=message)

	def toString(self, attrs: typing.MutableMapping[str, str] = {}) -> str:
		entities = ["{}=\"{}\"".format(key, value) for key, value in attrs.items() if value]
		return "<{}/>".format(" ".join([type(self).__name__] + entities))

	def __repr__(self) -> str:
		"""
		Human readable string representation of a result.
		"""
		return self.toString()

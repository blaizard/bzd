import typing
from pathlib import Path

from bzd.parser.error import Error
from bzd.parser.element import Element
from bzd.parser.context import Context

class SymbolMap:

	def __init__(self):
		self.map: typing.Dict[str, typing.Any] = {}
		# Memoized elements
		self.elements: typing.Dict[str, Element] = {}

	def insert(self, fqn: str, path: Path, element: Element, category: str) -> None:
		"""
		Insert a new element into the symbol map.
		Args:
			fqn: Full qualified name.
			path: Path associated with the element.
			element: Element to be registered.
			category: Category associated with the element, will be used for filtering.
		"""

		Error.assertTrue(element=element, condition=(fqn not in self.map), message="Symbol name is in conflict with a previous one {}.".format(fqn))
		self.map[fqn] = {"c": category, "p": path.as_posix(), "e": element.serialize()}

	def update(self, symbols: "SymbolMap") -> None:
		"""
		Register multiple symbols.
		"""
		for fqn, element in symbols.map.items():
			assert fqn not in self.map, "Symbol '{}' already defined.".format(fqn)
			self.map[fqn] = element

	def serialize(self) -> typing.Dict[str, typing.Any]:
		"""
		Return a serialized version of this map.
		"""
		return self.map

	def getElement(self, key: str, category: typing.Optional[str] = None) -> typing.Optional[Element]:
		"""
		Return an element from the symbol map.
		"""

		if key not in self.map:
			return None

		if category is not None and self.map[key]["c"] != category:
			return None

		# Not memoized
		if key not in self.elements:
			element = Element.fromSerialize(element=self.map[key]["e"], context=Context(path=Path(self.map[key]["p"])))
			self.elements[key] = element

		# Return the element
		return self.elements[key]

	@staticmethod
	def makeFQN(name: str, namespace: typing.List[str]) -> str:
		"""
		Make the fully qualified name from a symbol name
		"""
		return ".".join(namespace + [name])

	def getElementFromName(self, name: str, namespace: typing.List[str], category: typing.Optional[str] = None) -> typing.Optional[Element]:
		"""
		Return an element from an unqualified name.
		"""

		# Look for a symbol matchparsed
		namespace = namespace.copy()
		while True:
			fqn = SymbolMap.makeFQN(name=name, namespace=namespace)
			if fqn in self.map:
				break
			if not namespace:
				return None
			namespace.pop()

		# Match found
		return self.getElement(key=fqn, category=category)

	@staticmethod
	def fromSerialize(data: typing.Dict[str, typing.Any]) -> "SymbolMap":
		"""
		Create a symbol map from a serialized object.
		"""
		symbols = SymbolMap()
		symbols.map = data
		return symbols

	def __repr__(self) -> str:
		content = []
		for key, data in self.map.items():
			content.append("[{}] {}".format(data["c"], key))
		return "\n".join(content)

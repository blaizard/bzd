import typing
from pathlib import Path

from bzd.parser.error import Error
from bzd.parser.element import Element
from bzd.parser.context import Context

from tools.bdl.visitor import Visitor
from tools.bdl.builtins import Builtins

class SymbolMap:

	def __init__(self):
		self.map: typing.Dict[str, typing.Any] = {}
		self.builtins: typing.Dict[str, typing.Any] = {}
		# Memoized elements
		self.elements: typing.Dict[str, Element] = {}

		# Register builtins
		for builtin in Builtins:
			self.builtins[builtin.name] = {"c": "builtin", "p": "", "e": builtin.element.serialize()}

	def _contains(self, fqn: str) -> bool:
		"""
		Check if the fqn is registered.
		"""
		return self._get(fqn) is not None

	def _get(self, fqn: str) -> typing.Optional[typing.Any]:
		"""
		Return the raw data if it exsits.
		"""
		if fqn in self.map:
			return self.map[fqn]
		if fqn in self.builtins:
			return self.builtins[fqn]
		return None

	def insert(self, fqn: str, path: Path, element: Element, category: str) -> None:
		"""
		Insert a new element into the symbol map.
		Args:
			fqn: Full qualified name.
			path: Path associated with the element.
			element: Element to be registered.
			category: Category associated with the element, will be used for filtering.
		"""
		Error.assertTrue(element=element, condition=(not self._contains(fqn)), message="Symbol name is in conflict with a previous one: '{}'.".format(fqn))
		self.map[fqn] = {"c": category, "p": path.as_posix(), "e": element.serialize()}

	def update(self, symbols: "SymbolMap") -> None:
		"""
		Register multiple symbols.
		"""
		for fqn, element in symbols.map.items():
			assert not self._contains(fqn), "Symbol '{}' already defined.".format(fqn)
			self.map[fqn] = element

	def serialize(self) -> typing.Dict[str, typing.Any]:
		"""
		Return a serialized version of this map.
		"""
		return self.map

	@staticmethod
	def makeFQN(name: str, namespace: typing.List[str]) -> str:
		"""
		Make the fully qualified name from a symbol name
		"""
		return ".".join(namespace + [name])

	def resolveFQN(self, name: str, namespace: typing.List[str]) -> typing.Optional[str]:
		"""
		Find the fully qualified name of a given a name and a namespace.
		"""
		# Look for a symbol match
		namespace = namespace.copy()
		while True:
			fqn = SymbolMap.makeFQN(name=name, namespace=namespace)
			if self._contains(fqn):
				break
			if not namespace:
				return None
			namespace.pop()

		return fqn

	def getElement(self, fqn: str, category: typing.Optional[str] = None) -> typing.Optional[Element]:
		"""
		Return an element from the symbol map.
		"""

		data = self._get(fqn)
		if data is None:
			return None

		# Not memoized
		if fqn not in self.elements:
			element = Element.fromSerialize(element=data["e"], context=Context(path=Path(data["p"])))
			self.elements[fqn] = element

		# Return the element
		return self.elements[fqn]

	def getElementFromName(self, name: str, namespace: typing.List[str], category: typing.Optional[str] = None) -> typing.Optional[Element]:
		"""
		Return an element from an unqualified name.
		"""

		# Look for a symbol match
		fqn = self.resolveFQN(name=name, namespace=namespace)
		if fqn is None:
			return None

		# Match found
		return self.getElement(fqn=fqn, category=category)

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
		for key, data in {**self.map, **self.builtins}.items():
			content.append("[{}] {}".format(data["c"], key))
		return "\n".join(content)

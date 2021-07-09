import typing
from pathlib import Path

from bzd.parser.error import Error
from bzd.parser.element import Element
from bzd.parser.context import Context

from tools.bdl.visitor import Visitor
from tools.bdl.builtins import Builtins
from tools.bdl.entities.all import elementToEntity, EntityType


class SymbolMap:

	def __init__(self) -> None:
		self.map: typing.Dict[str, typing.Any] = {}
		self.builtins: typing.Dict[str, typing.Any] = {}
		# Memoized entities
		self.entities: typing.Dict[str, EntityType] = {}

		# Register builtins
		for builtin in Builtins:
			self.builtins[builtin.name] = {"c": "builtin", "p": "", "e": builtin.element.serialize()}

	def _contains(self, fqn: str, exclude: typing.Optional[typing.List[str]] = None) -> bool:
		"""
		Check if the fqn is registered.
		"""
		maybeData = self._get(fqn=fqn)
		if maybeData is None:
			return False
		if exclude:
			return maybeData["c"] not in exclude
		return True

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
		Error.assertTrue(element=element,
			condition=(not self._contains(fqn=fqn)),
			message="Symbol name is in conflict with a previous one: '{}'.".format(fqn))
		self.map[fqn] = {"c": category, "p": path.as_posix(), "e": element.serialize()}

	def update(self, symbols: "SymbolMap") -> None:
		"""
		Register multiple symbols.
		"""
		for fqn, element in symbols.map.items():
			assert not self._contains(fqn=fqn), "Symbol '{}' already defined.".format(fqn)
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

	def resolveFQN(self,
		name: str,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> typing.Optional[str]:
		"""
		Find the fully qualified name of a given a name and a namespace.
		"""
		# Look for a symbol match
		namespace = namespace.copy()
		while True:
			fqn = SymbolMap.makeFQN(name=name, namespace=namespace)
			if self._contains(fqn=fqn, exclude=exclude):
				break
			if not namespace:
				return None
			namespace.pop()

		return fqn

	def getEntity(self, fqn: str, category: typing.Optional[str] = None) -> typing.Optional[EntityType]:
		"""
		Return an element from the symbol map.
		"""

		data = self._get(fqn)
		if data is None:
			return None

		# Not memoized
		if fqn not in self.entities:
			element = Element.fromSerialize(element=data["e"], context=Context(path=Path(data["p"])))
			entity = elementToEntity(element=element)
			self.entities[fqn] = entity

		# Return the element
		return self.entities[fqn]

	def getEntityFromName(self,
		name: str,
		namespace: typing.List[str],
		category: typing.Optional[str] = None) -> typing.Optional[EntityType]:
		"""
		Return an element from an unqualified name.
		"""

		# Look for a symbol match
		fqn = self.resolveFQN(name=name, namespace=namespace)
		if fqn is None:
			return None

		# Match found
		return self.getEntity(fqn=fqn, category=category)

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

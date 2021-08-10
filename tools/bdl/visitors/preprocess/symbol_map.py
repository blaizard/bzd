import typing
from pathlib import Path

from bzd.parser.error import Error, Result
from bzd.parser.element import Element, ElementBuilder, SequenceBuilder
from bzd.parser.context import Context

from tools.bdl.visitor import Visitor, CATEGORIES
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
			self.entities[builtin.name] = builtin

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

	def insert(
			self,
			fqn: str,
			path: typing.Optional[Path],
			element: Element,
			category: str,
			conflicts: bool = False) -> None:
		"""
		Insert a new element into the symbol map.
		Args:
			fqn: Full qualified name.
			path: Path associated with the element.
			element: Element to be registered.
			category: Category associated with the element, will be used for filtering.
			conflicts: Handle symbol FQN conflicts.
		"""
		if self._contains(fqn=fqn):
			if not conflicts or element != self.getEntityResolved(fqn=fqn).assertValue(element=element).element:
				Error.handleFromElement(element=element,
					message="Symbol name is in conflict with a previous one: '{}'.".format(fqn))

		self.map[fqn] = {"c": category, "p": path.as_posix() if path is not None else "", "e": None}
		# The element is also added to the entity map, to allow further modification that will
		# be written when serialize is called.
		self.entities[fqn] = elementToEntity(element=element)

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

		# Create serialized blobs for elements present in the entities map.
		for fqn, entity in self.entities.items():

			# Ignore builtins
			if fqn in self.builtins:
				continue

			entity.assertTrue(condition=fqn in self.map,
				message="Entry '{}' was not properly created before being added to the entities map.".format(fqn))
			element = entity.element

			# Remove nested element and change them to references.
			if any([element.isNestedSequence(category) for category in CATEGORIES]):

				preparedElement = element.copy(ignoreNested=CATEGORIES)
				for category in CATEGORIES:
					nested = element.getNestedSequence(category)
					if nested:
						sequence = SequenceBuilder()
						for element in nested:
							if element.isAttr("name"):
								sequence.pushBackElement(ElementBuilder().addAttr("category",
									"reference").addAttr(key="name",
									value=SymbolMap.namespaceToFQN(name=element.getAttr("name").value,
									namespace=SymbolMap.FQNToNamespace(fqn)),
									index=element.getAttr("name").index))
						preparedElement.setNestedSequence(category, sequence)
				element = preparedElement

			# Serialize the element to the map.
			self.map[fqn]["e"] = element.serialize()

		# Sanity check to ensure that all entries in the map are valid.
		for fqn, entry in self.map.items():
			assert "e" in entry and entry["e"], "Invalid element in the map: {}.".format(entry)
			assert "c" in entry and entry["c"], "Invalid category in the map: {}.".format(entry)
			assert "p" in entry, "Missing path in the map: {}.".format(entry)

		return self.map

	@staticmethod
	def namespaceToFQN(namespace: typing.List[str], name: typing.Optional[str] = None) -> str:
		"""
		Make the fully qualified name from a symbol name
		"""
		if name is None:
			return ".".join(namespace)
		return ".".join(namespace + [name])

	@staticmethod
	def FQNToNamespace(fqn: str) -> typing.List[str]:
		"""
		Convert a FQN string into a namespace.
		"""
		return fqn.split(".")

	def resolveFQN(
			self,
			name: str,
			namespace: typing.List[str],
			exclude: typing.Optional[typing.List[str]] = None) -> Result[str]:
		"""
		Find the fully qualified name of a given a name and a namespace.
		Note, name can be a partial fqn.
		"""
		nameFirst = SymbolMap.FQNToNamespace(name)[0]

		# Look for a symbol match of the first part of the name.
		namespace = namespace.copy()
		while True:
			fqn = SymbolMap.namespaceToFQN(name=nameFirst, namespace=namespace)
			if self._contains(fqn=fqn, exclude=exclude):
				break
			if not namespace:
				return Result[str].makeError("Symbol '{}' in namespace '{}' could not be resolved.".format(
					name, ".".join(namespace)) if namespace else "Symbol '{}' could not be resolved.".format(name))
			namespace.pop()

		# If match, ensure that the rest of the name also matches with the namespace identified.
		if name != nameFirst:
			fqn = SymbolMap.namespaceToFQN(name=name, namespace=namespace)

		return Result[str](fqn)

	def getEntityResolved(self, fqn: str, category: typing.Optional[str] = None) -> Result[EntityType]:
		"""
		Return an element from the symbol map.
		This call assumes that FQN is already resolved.
		"""
		data = self._get(fqn)
		if data is None:
			return Result[EntityType].makeError("Unable to find symbol '{}'.".format(fqn))

		# Not memoized
		if fqn not in self.entities:
			element = Element.fromSerialize(element=data["e"], context=Context(path=Path(data["p"])))
			entity = elementToEntity(element=element)
			self.entities[fqn] = entity
			# Resolve dependencies
			for category in CATEGORIES:
				if element.isNestedSequence(category):
					updatedSequence = SequenceBuilder()
					for nested in element.getNestedSequenceAssert(category):
						# There should be only references
						Error.assertTrue(element=nested,
							condition=nested.isAttr("category"),
							message="All sub-elements must have a category.")
						Error.assertTrue(element=nested,
							condition=(nested.getAttr("category").value == "reference"),
							message="All sub-elements must be of type 'reference'.")
						Error.assertTrue(element=nested,
							condition=nested.isAttr("name"),
							message="Reference is missing attribute 'name'.")
						nestedEntity = self.getEntityResolved(fqn=nested.getAttr("name").value).assertValue(
							element=nested)
						updatedSequence.pushBackElement(nestedEntity.element)
					ElementBuilder.cast(element, ElementBuilder).setNestedSequence(kind=category,
						sequence=updatedSequence)

		# Return the element
		return Result[EntityType](self.entities[fqn])

	def getEntity(self, fqn: str, category: typing.Optional[str] = None) -> Result[EntityType]:
		"""
		Return an element from the symbol map and resolves underlying types if needed.
		"""

		namespace = []
		result = None
		for name in SymbolMap.FQNToNamespace(fqn):
			namespace.append(name)
			result = self.getEntityResolved(fqn=SymbolMap.namespaceToFQN(namespace=namespace), category=category)
			if not result:
				continue
			if result.value.underlyingType is not None:
				namespace = SymbolMap.FQNToNamespace(result.value.underlyingType)

		if not result:
			return Result.makeError("Could not resolve symbol '{}'.".format(fqn))

		return Result(result.value)

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
			content.append("[{}] {}: {}".format(data["c"], key, data["e"]))
		return "\n".join(content)

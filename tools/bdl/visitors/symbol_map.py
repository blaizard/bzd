import typing
from pathlib import Path

from bzd.parser.error import Error, Result
from bzd.parser.element import Element, SequenceBuilder
from bzd.parser.context import Context

from tools.bdl.visitor import Visitor, CATEGORIES
from tools.bdl.builtins import Builtins
from tools.bdl.entities.all import elementToEntity, EntityType
from tools.bdl.entities.builder import ElementBuilder
from tools.bdl.entities.impl.reference import Reference


class SymbolMap:
	"""
	Symbol map looks like this:
	 - bzd: <namespace "bzd">                         <- Created implicitly by "bzd.nested" namespace
	 - bzd.nested: <namespace "bzd.nested">
	 - bzd.nested.Struct: <nested>
	                          <reference "bzd.nested.Struct.a">
	                          <reference "bzd.nested.Struct.b">
	                          <expression> <- unamed expression
	 - bzd.nested.Struct.a: <expression "a">
	 - bzd.nested.Struct.b: <expression "b">
	 - bzd.Child: <nested>
	                 <reference "bzd.Child.c">
	 - bzd.Child.c: <expression "c">
	 - bzd.Child.a: <reference "bzd.nested.Struct.a">
	 - bzd.Child.b: <reference "bzd.nested.Struct.b">
	 - _0~: <nested>                                  <- unamed top level element
	"""

	def __init__(self) -> None:
		self.staticUid: int = 0
		self.map: typing.Dict[str, typing.Any] = {}
		self.builtins: typing.Dict[str, typing.Any] = {}
		# Memoized entities
		self.entities: typing.Dict[str, EntityType] = {}

		# Register builtins
		for builtin in Builtins:
			self.builtins[builtin.name] = {"c": "builtin", "p": "", "e": builtin.element.serialize()}
			self.entities[builtin.name] = builtin

	def contains(self,
		fqn: str,
		exclude: typing.Optional[typing.List[str]] = None,
		categories: typing.Optional[typing.Set[str]] = None) -> bool:
		"""
		Check if the fqn is registered.
		"""
		maybeData = self._get(fqn=fqn)
		if maybeData is None:
			return False
		if exclude:
			return maybeData["c"] not in exclude
		if categories:
			return maybeData["c"] in categories
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

	def items(self, categories: typing.Set[str] = set()) -> typing.Iterator[typing.Tuple[str, EntityType]]:
		"""
		Iterate through entities optionaly filtered by their categories.
		"""

		for fqn, meta in self.map.items():
			if meta["c"] in categories:
				entity = self.getEntityResolved(fqn=fqn).value
				yield fqn, entity

	def insert(self,
		fqn: typing.Optional[str],
		path: typing.Optional[Path],
		element: Element,
		category: str,
		conflicts: bool = False) -> str:
		"""
		Insert a new element into the symbol map.
		Args:
			fqn: Full qualified name.
			path: Path associated with the element.
			element: Element to be registered.
			category: Category associated with the element, will be used for filtering.
			conflicts: Handle symbol FQN conflicts.
		"""
		if fqn is None:
			# The '_' is to tell that the visibility of this object is limited to this file,
			# while the '~' is to ensure no name collision with what the user can define.
			fqn = "_{:d}~".format(self.staticUid)
			self.staticUid += 1
		assert fqn is not None

		# Save the FQN to the element, so that it can be found during [de]serialization
		ElementBuilder.cast(element, ElementBuilder).setAttr("fqn", fqn)

		if self.contains(fqn=fqn):
			originalElement = self.getEntityResolved(fqn=fqn).assertValue(element=element).element
			if not conflicts or element != originalElement:
				SymbolMap.errorSymbolConflict_(element, originalElement)

		self.map[fqn] = {"c": category, "p": path.as_posix() if path is not None else "", "e": None}

		# Resolve context
		context, _, _ = element.context.resolve()
		element.context = context

		# The element is also added to the entity map, to allow further modification that will
		# be written when serialize is called.
		self.entities[fqn] = elementToEntity(element=element)

		return fqn

	def insertInheritance(self, fqn: str, fqnInheritance: str, category: str) -> None:
		"""
		Copy all nested references from the inheritance to the passed fqn.
		"""
		inheritance = self.getEntityResolved(fqn=fqnInheritance).value
		namespaceInheritance = SymbolMap.FQNToNamespace(fqnInheritance)
		namespace = SymbolMap.FQNToNamespace(fqn)
		for nested in inheritance.nested:
			if nested.isName:
				fqnNested = SymbolMap.namespaceToFQN(name=nested.name, namespace=namespaceInheritance)
				fqnTarget = SymbolMap.namespaceToFQN(name=nested.name, namespace=namespace)
				if not self.contains(fqn=fqnTarget):
					self.insert(fqn=fqnTarget, path=None, element=self.makeReference(fqn=fqnNested), category=category)

	@staticmethod
	def errorSymbolConflict_(element1: Element, element2: Element) -> None:
		message = Error.handleFromElement(element=element1, message="Symbol name is in conflict...", throw=False)
		message += Error.handleFromElement(element=element2, message="...with this one.", throw=False)
		raise Exception(message)

	@staticmethod
	def isPrivate(fqn: str) -> bool:
		return fqn.startswith("_")

	def update(self, symbols: "SymbolMap") -> None:
		"""
		Register multiple symbols.
		"""
		for fqn, element in symbols.map.items():
			# Ignore private entries
			if SymbolMap.isPrivate(fqn):
				continue
			existingElement = self._get(fqn=fqn)
			if existingElement is not None and element["p"] != existingElement["p"]:
				SymbolMap.errorSymbolConflict_(SymbolMap.metaToElement(element),
					SymbolMap.metaToElement(existingElement))
			self.map[fqn] = element

	def makeReference(self, fqn: str, category: typing.Optional[str] = None) -> Element:
		"""
		Create a reference from an existing FQN.
		"""
		assert self.contains(fqn=fqn), "The FQN '{}' is not part of the symbol map.".format(fqn)
		return ElementBuilder("reference").setAttr(key="name", value=fqn)

	def serialize(self) -> typing.Dict[str, typing.Any]:
		"""
		Return a serialized version of this map.
		"""

		# Create serialized blobs for elements present in the entities map.
		removeFQNs: typing.Set[str] = set()
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
							Error.assertHasAttr(element=element, attr="fqn")
							fqnNested = element.getAttr("fqn").value
							# Remove private FQNs and keep them nested
							if SymbolMap.isPrivate(fqnNested):
								removeFQNs.add(fqnNested)
								sequence.pushBackElement(element)
							else:
								sequence.pushBackElement(self.makeReference(fqnNested))
						preparedElement.setNestedSequence(category, sequence)
				element = preparedElement

			# Serialize the element to the map.
			self.map[fqn]["e"] = element.serialize()

		# Remove some of the FQNs that have been copied.
		for fqn in removeFQNs:
			del self.map[fqn]

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
			if self.contains(fqn=fqn, exclude=exclude):
				break
			if not namespace:
				return Result[str].makeError("Symbol '{}' in namespace '{}' could not be resolved.".format(
					name, ".".join(namespace)) if namespace else "Symbol '{}' could not be resolved.".format(name))
			namespace.pop()

		# If match, ensure that the rest of the name also matches with the namespace identified.
		if name != nameFirst:
			fqn = SymbolMap.namespaceToFQN(name=name, namespace=namespace)

		return Result[str](fqn)

	@staticmethod
	def metaToElement(meta: typing.Any) -> Element:
		return Element.fromSerialize(element=meta["e"], context=Context(path=Path(meta["p"])))

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
			element = SymbolMap.metaToElement(data)
			entity = elementToEntity(element=element)
			# Resolve dependencies
			for category in CATEGORIES:
				if element.isNestedSequence(category):
					updatedSequence = SequenceBuilder()
					for nested in element.getNestedSequenceAssert(category):
						nestedEntity = elementToEntity(nested)
						# Resolve the reference if any
						if isinstance(nestedEntity, Reference):
							nestedEntity = self.getEntityResolved(fqn=nestedEntity.name,
								category=category).assertValue(element=element)
						updatedSequence.pushBackElement(nestedEntity.element)
					ElementBuilder.cast(element, ElementBuilder).setNestedSequence(kind=category,
						sequence=updatedSequence)
			self.entities[fqn] = entity

		# Return the referenced type in case of reference.
		if isinstance(self.entities[fqn], Reference):
			return self.getEntityResolved(fqn=self.entities[fqn].name, category=category)

		# Return the entity.
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

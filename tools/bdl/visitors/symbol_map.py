import typing
from pathlib import Path

from bzd.parser.error import Error, Result
from bzd.parser.element import Element, SequenceBuilder
from bzd.parser.context import Context

from tools.bdl.visitor import Visitor, CATEGORIES, CATEGORY_COMPOSITION, CATEGORY_GLOBAL_COMPOSITION
from tools.bdl.builtins import Builtins
from tools.bdl.entities.all import elementToEntity, EntityType
from tools.bdl.entities.builder import ElementBuilder
from tools.bdl.entities.impl.reference import Reference
from tools.bdl.entities.impl.fragment.fqn import FQN

ResolveShallowFQNResult = Result[typing.Tuple[str, typing.List[str]]]
resolveFQNResult = Result[typing.List[str]]


class Resolver:

	def __init__(self,
		symbols: "SymbolMap",
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None,
		this: typing.Optional[str] = None) -> None:
		self.symbols = symbols
		self.namespace = namespace
		self.exclude = exclude
		self.this = this

	def makeFQN(self, name: str) -> str:
		"""
		Create an FQN out of a name.
		"""
		return FQN.fromNamespace(name=name, namespace=self.namespace)

	def getEntity(self, fqn: str) -> Result[EntityType]:
		return self.symbols.getEntity(fqn=fqn, exclude=self.exclude)

	def getEntityResolved(self, fqn: str) -> Result[EntityType]:
		return self.symbols.getEntityResolved(fqn=fqn, exclude=self.exclude)

	def resolveShallowFQN(self, name: str) -> ResolveShallowFQNResult:
		"""
		Find the fully qualified name of a given a name and a namespace.
		Note, name can be a partial fqn.
		"""
		nameFirst = FQN.toNamespace(name)[0]

		if nameFirst == "this":
			if self.this is None:
				return ResolveShallowFQNResult.makeError("Keyword 'this' must be used in an object context.")
			nameFirst = self.this

		# Look for a symbol match of the first part of the name.
		potentialNamespace = self.namespace.copy()
		while True:
			fqn = FQN.fromNamespace(name=nameFirst, namespace=potentialNamespace)
			if self.symbols.contains(fqn=fqn, exclude=self.exclude):
				break
			if not potentialNamespace:
				return ResolveShallowFQNResult.makeError(
					"Symbol '{}' in namespace '{}' could not be resolved.".format(nameFirst, ".".join(self.
					namespace)) if self.namespace else "Symbol '{}' could not be resolved.".format(nameFirst))
			potentialNamespace.pop()

		# Attempt to resolve as much as possible.
		remainingNamespace = FQN.toNamespace(name)[1:]
		while remainingNamespace:
			potentialFQN = FQN.fromNamespace(name=remainingNamespace[0], namespace=FQN.toNamespace(fqn))
			if not self.symbols.contains(fqn=potentialFQN, exclude=self.exclude):
				break
			remainingNamespace.pop(0)
			fqn = potentialFQN

		# Return the FQN and the unresolved rest.
		return ResolveShallowFQNResult((fqn, remainingNamespace))

	def resolveFQN(self, name: str) -> resolveFQNResult:
		"""
		Find the fully qualified name of a given a name and a namespace.
		Note, name can be a partial fqn.
		"""
		maybeFQN = self.resolveShallowFQN(name=name)
		if not maybeFQN:
			return resolveFQNResult.makeError(maybeFQN.error)

		fqn: typing.Optional[str]
		fqn, unresolvedFQN = maybeFQN.value

		# List of matching FQNs
		fqns: typing.List[str] = [fqn]

		# Re-iterate the process with the next items
		for nextName in unresolvedFQN:
			entity = self.getEntityResolved(fqn=fqn).value
			potentialNamespaceFQNs = [fqn]
			# If it has an underlying type, add it to the list as well as its parents (if any).
			if entity.underlyingType is not None:
				potentialNamespaceFQNs += [entity.underlyingType
											] + entity.getEntityUnderlyingTypeResolved(resolver=self).getParents()
			# Check if there is any match.
			fqn = None
			for potentialNamespaceFQN in potentialNamespaceFQNs:
				potentialFQN = FQN.fromNamespace(name=nextName, namespace=FQN.toNamespace(potentialNamespaceFQN))
				if self.symbols.contains(fqn=potentialFQN, exclude=self.exclude):
					fqn = potentialFQN
					break
			if fqn is None:
				return resolveFQNResult.makeError("Symbol '{}' from '{}' in namespace '{}' could not be resolved.".
					format(nextName, name, ".".join(self.namespace)) if self.
					namespace else "Symbol '{}' from '{}' could not be resolved.".format(nextName, name))
			fqns.append(fqn)

		# Return the final FQN.
		return resolveFQNResult(fqns)


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
	 - _0~: <nested>                                  <- unamed top level element
	 - 321332-323-3~: <expression>                    <- unamed composition expression
	"""

	def __init__(self) -> None:
		self.staticUid: int = 0
		self.map: typing.Dict[str, typing.Any] = {}
		self.builtins: typing.Dict[str, typing.Any] = {}
		# Memoized entities.
		self.entities: typing.Dict[str, EntityType] = {}
		self.isClosed = False

		# Register builtins
		for builtin in Builtins:
			self.insertBuiltin(name=builtin.name, entity=builtin)

	def makeResolver(self, *args: typing.Any, **kwargs: typing.Any) -> Resolver:
		"""
		Create a resolver object, used to find (resolve) symbols.
		"""
		return Resolver(self, *args, **kwargs)

	def insertBuiltin(self, name: str, entity: EntityType) -> None:
		"""
		Insert a builtin entry to the mix.
		"""
		ElementBuilder.cast(entity.element, ElementBuilder).setAttr("fqn", name)
		self.builtins[name] = {"c": "builtin", "p": "", "e": entity.element.serialize()}
		self.entities[name] = entity

	def contains(self, fqn: str, exclude: typing.Optional[typing.List[str]] = None) -> bool:
		"""
		Check if the fqn is registered.
		"""
		return False if self._get(fqn=fqn, exclude=exclude) is None else True

	def _get(self, fqn: str, exclude: typing.Optional[typing.List[str]] = None) -> typing.Optional[typing.Any]:
		"""
		Return the raw data if it exsits.
		"""
		if fqn in self.map:
			data = self.map[fqn]
		elif fqn in self.builtins:
			data = self.builtins[fqn]
		else:
			return None
		if exclude and data["c"] in exclude:
			return None
		return data

	def items(self, categories: typing.Set[str] = set()) -> typing.Iterator[typing.Tuple[str, EntityType]]:
		"""
		Iterate through entities optionaly filtered by their categories.
		"""

		for fqn, meta in self.map.items():
			if meta["c"] in categories:
				entity = self.getEntityResolved(fqn=fqn).value
				yield fqn, entity

	def insert(self,
		name: typing.Optional[str],
		namespace: typing.List[str],
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
		if name is None:
			# These are the unnamed elements that should be progpagated to other translation units.
			if category in {CATEGORY_COMPOSITION, CATEGORY_GLOBAL_COMPOSITION}:
				fqn = FQN.makeUnique(namespace=namespace)
			# If not, they will be kept private.
			else:
				fqn = FQN.makeUniquePrivate()
		else:
			# Build the symbol name.
			fqn = FQN.fromNamespace(name=name, namespace=namespace)
		assert fqn is not None

		# Save the FQN to the element, so that it can be found during [de]serialization.
		# This is also used to refer to the element with the symbol tree, the top-level ones only.
		# In addition, it is assumed that during while resolved any element have a valid FQN.
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

	@staticmethod
	def errorSymbolConflict_(element1: Element, element2: Element) -> None:
		message = Error.handleFromElement(element=element1, message="Symbol name is in conflict...", throw=False)
		message += Error.handleFromElement(element=element2, message="...with this one.", throw=False)
		raise Exception(message)

	def update(self, symbols: "SymbolMap") -> None:
		"""
		Register multiple symbols.
		"""
		for fqn, element in symbols.map.items():

			# Ignore private entries
			if FQN.isPrivate(fqn):
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

	def close(self) -> None:
		"""
		Close the map to prevent any further editing.
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
					if nested is not None:
						sequence = SequenceBuilder()
						for nestedElement in nested:
							if nestedElement.isAttr("fqn"):
								fqnNested = nestedElement.getAttr("fqn").value
								# Remove private FQNs and keep them nested.
								# This is needed because when merging nested structure, some config
								# for example have unamed elements and need to be copied with the rest.
								if FQN.isPrivate(fqnNested):
									removeFQNs.add(fqnNested)
									ElementBuilder.cast(nestedElement, ElementBuilder).removeAttr("fqn")
									sequence.pushBackElement(nestedElement)
								else:
									sequence.pushBackElement(self.makeReference(fqnNested))
							else:
								sequence.pushBackElement(nestedElement)
						if sequence:
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

		# Mark as closed.
		self.isClosed = True

	def serialize(self) -> typing.Dict[str, typing.Any]:
		"""
		Return a serialized version of this map.
		"""
		assert self.isClosed, "Can only be serialized after being closed."
		return self.map

	@staticmethod
	def metaToElement(meta: typing.Any) -> Element:
		return Element.fromSerialize(element=meta["e"], context=Context(path=Path(meta["p"])))

	def getEntityResolved(self, fqn: str, exclude: typing.Optional[typing.List[str]] = None) -> Result[EntityType]:
		"""
		Return an element from the symbol map.
		This call assumes that FQN is already resolved.
		"""

		data = self._get(fqn=fqn, exclude=exclude)
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
								exclude=exclude).assertValue(element=element)
						updatedSequence.pushBackElement(nestedEntity.element)
					ElementBuilder.cast(element, ElementBuilder).setNestedSequence(kind=category,
						sequence=updatedSequence)
			self.entities[fqn] = entity

		# Return the referenced type in case of reference.
		if isinstance(self.entities[fqn], Reference):
			return self.getEntityResolved(fqn=self.entities[fqn].name, exclude=exclude)

		# Return the entity.
		return Result[EntityType](self.entities[fqn])

	def getEntity(self, fqn: str, exclude: typing.Optional[typing.List[str]] = None) -> Result[EntityType]:
		"""
		Return an element from the symbol map and resolves underlying types if needed.
		"""

		namespace = []
		result = None
		for name in FQN.toNamespace(fqn):
			namespace.append(name)
			result = self.getEntityResolved(fqn=FQN.fromNamespace(namespace=namespace), exclude=exclude)
			if not result:
				continue
			if result.value.underlyingType is not None:
				namespace = FQN.toNamespace(result.value.underlyingType)

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
			content.append("[{}] {}".format(data["c"], key))
		return "\n".join(content)

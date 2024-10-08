import typing
from pathlib import Path

from bzd.parser.error import Error, Result
from bzd.parser.element import Element, SequenceBuilder
from bzd.parser.context import Context
from bzd.utils.match import sortMatchingWeight

from bdl.visitor import Visitor, Group, NestedSequence
from bdl.builtins import Builtins
from bdl.entities.all import elementToEntity, EntityType
from bdl.entities.builder import ElementBuilder
from bdl.entities.impl.reference import Reference
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.expression import Expression
from bdl.entities.impl.types import Category

ResolveShallowFQNResult = Result[typing.Tuple[str, typing.List[str]]]
ResolveFQNResult = Result[typing.List[str]]


class Resolver:

	def __init__(
	    self,
	    symbols: "SymbolMap",
	    namespace: typing.List[str] = [],
	    exclude: typing.Optional[typing.List[Group]] = None,
	    this: typing.Optional[str] = None,
	    target: typing.Optional[str] = None,
	    memoize: bool = False,
	) -> None:
		self.symbols = symbols
		self.namespace = namespace
		self.exclude = exclude
		self.this = this
		self.target = target
		self.memoize = memoize

	def make(
	    self,
	    namespace: typing.Optional[typing.List[str]] = None,
	    expression: typing.Optional[Expression] = None,
	    this: typing.Optional[str] = None,
	) -> "Resolver":
		"""Create a new resolver with an updated namespace.

        Note: exclude and this should not be propagated, they both concern the first level resolver.
        """

		if namespace is not None:
			assert expression is None
			return Resolver(
			    symbols=self.symbols,
			    namespace=namespace,
			    this=this,
			    target=self.target,
			    memoize=self.memoize,
			)
		assert expression is not None
		return expression.makeResolver(symbols=self.symbols, target=self.target, memoize=self.memoize)

	def makeFQN(self, name: str) -> str:
		"""Create an FQN out of a name."""

		return FQN.fromNamespace(name=name, namespace=self.namespace)

	def getEntity(self, fqn: str) -> Result[EntityType]:
		return self.symbols.getEntity(fqn=fqn, exclude=self.exclude)

	def getEntityResolved(self, fqn: str) -> Result[EntityType]:
		if fqn.startswith("target."):
			return self.symbols.getEntityResolved(fqn="Any")
		return self.symbols.getEntityResolved(fqn=fqn, exclude=self.exclude)

	def resolveShallowFQN(self, name: str) -> ResolveShallowFQNResult:
		"""Find the fully qualified name of a given a name and a namespace.

        Note, name can be a partial fqn.
        """
		nameFirst = FQN.toNamespace(name)[0]

		if nameFirst == "this":
			if self.this is None:
				return ResolveShallowFQNResult.makeError("Keyword 'this' must be used in an object context.")
			fqn = self.this

		elif nameFirst == "target":
			if self.target is None:
				return ResolveShallowFQNResult((name, []))
			fqn = self.target

		else:
			# Look for a symbol match of the first part of the name, only if `nameFirst` is not a special name.
			potentialNamespace = self.namespace.copy()
			while True:
				fqn = FQN.fromNamespace(name=nameFirst, namespace=potentialNamespace)
				if self.symbols.contains(fqn=fqn, exclude=self.exclude):
					break
				if not potentialNamespace:
					ending = self.symbols._terminateErrorMessageSimilarFQN(fqn=nameFirst)
					return ResolveShallowFQNResult.makeError(
					    f"Symbol '{nameFirst}' in namespace '{'.'.join(self.namespace)}' could not be resolved{ending}"
					    if self.namespace else f"Symbol '{nameFirst}' could not be resolved{ending}")
				potentialNamespace.pop()

		# Attempt to resolve as much as possible.
		potentialNamespace = FQN.toNamespace(fqn) + FQN.toNamespace(name)[1:]
		remainingNamespace: typing.List[str] = []
		while potentialNamespace:
			fqn = FQN.fromNamespace(namespace=potentialNamespace)
			if self.symbols.contains(fqn=fqn, exclude=self.exclude):
				break
			remainingNamespace.insert(0, potentialNamespace.pop())

		# Return the FQN and the unresolved rest.
		return ResolveShallowFQNResult((fqn, remainingNamespace))

	def resolveFQN(self, name: str) -> ResolveFQNResult:
		"""
        Find the fully qualified name of a given a name and a namespace.
        Note, name can be a partial fqn.
        """
		maybeFQN = self.resolveShallowFQN(name=name)
		if not maybeFQN:
			return ResolveFQNResult.makeError(maybeFQN.error)

		fqn: typing.Optional[str]
		fqn, unresolvedFQN = maybeFQN.value

		# List of matching FQNs
		fqns: typing.List[str] = [fqn]

		# Re-iterate the process with the next items
		for nextName in unresolvedFQN:
			entity = self.getEntityResolved(fqn=fqn).value
			potentialNamespaceFQNs = [fqn]
			# If it has an underlying type, add it to the list as well as its parents (if any).
			if entity.underlyingTypeFQN is not None:
				potentialNamespaceFQNs += [entity.underlyingTypeFQN
				                           ] + entity.getEntityUnderlyingTypeResolved(resolver=self).getParents()
			# Check if there is any match.
			fqn = None
			for potentialNamespaceFQN in potentialNamespaceFQNs:
				potentialFQN = FQN.fromNamespace(name=nextName, namespace=FQN.toNamespace(potentialNamespaceFQN))
				if self.symbols.contains(fqn=potentialFQN, exclude=self.exclude):
					fqn = potentialFQN
					break
			if fqn is None:
				ending = self.symbols._terminateErrorMessageSimilarFQN(fqn=name)
				return ResolveFQNResult.makeError(
				    f"Symbol '{nextName}' from '{name}' in namespace '{'.'.join(self.namespace)}' could not be resolved{ending}"
				    if self.namespace else f"Symbol '{nextName}' from '{name}' could not be resolved{ending}")
			fqns.append(fqn)

		# Return the final FQN.
		return ResolveFQNResult(fqns)

	def __repr__(self) -> str:
		attrs = {
		    "namespace": self.namespace,
		    "this": self.this,
		    "exclude": self.exclude,
		}

		entities = ['{}="{}"'.format(key, value) for key, value in attrs.items() if value]
		return "<{}/>".format(" ".join([type(self).__name__] + entities))


class SymbolMap:
	"""
    Symbol map looks like this:
     - bzd: <namespace "bzd">                         <- Created implicitly by "bzd.nested" namespace
     - bzd.nested: <namespace "bzd.nested">
     - bzd.nested.Struct: <nested>
                              <reference "bzd.nested.Struct.a">
                              <reference "bzd.nested.Struct.b">
                              <expression> <- unnamed expression
     - bzd.nested.Struct.a: <expression "a">
     - bzd.nested.Struct.b: <expression "b">
     - bzd.Child: <nested>
                     <reference "bzd.Child.c">
     - bzd.Child.c: <expression "c">
     - _0~: <nested>                                  <- unnamed top level element
     - 321332-323-3~: <expression>                    <- unnamed composition expression
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
		# Resolve builtins before inserting them.
		entity.resolveMemoized(resolver=self.makeResolver())
		ElementBuilder.cast(entity.element, ElementBuilder).setAttr("fqn", name)
		self.builtins[name] = {
		    "g": Group.builtin.value,
		    "p": "",
		    "e": entity.element.serialize(),
		}
		self.entities[name] = entity

	def contains(self, fqn: str, exclude: typing.Optional[typing.List[Group]] = None) -> bool:
		"""
        Check if the fqn is registered.
        """
		return False if self._get(fqn=fqn, exclude=exclude) is None else True

	def _get(self, fqn: str, exclude: typing.Optional[typing.List[Group]] = None) -> typing.Optional[typing.Any]:
		"""
        Return the raw data if it exists.
        """
		if fqn in self.map:
			data = self.map[fqn]
		elif fqn in self.builtins:
			data = self.builtins[fqn]
		else:
			return None
		if exclude:
			for group in exclude:
				if group in Group(data["g"]):
					return None
		return data

	def items(self,
	          groups: typing.Set[Group],
	          startsWith: str = "",
	          depth: int = 0) -> typing.Iterator[typing.Tuple[str, EntityType]]:
		"""
        Iterate through entities optionally filtered by their groups.

        Args:
                groups: Groups to be returned.
                startsWith: The returned items start with the specified fqn.
                depth: depth level until which entity should be selected. A value of 0 means no limit.
        """

		for fqn, meta in self.map.items():
			if any(group in Group(meta["g"]) for group in groups):
				if fqn.startswith(startsWith):
					if depth == 0 or fqn[len(startsWith):].count(".") < depth:
						entity = self.getEntityResolved(fqn=fqn).value
						yield fqn, entity

	def insert(
	    self,
	    name: typing.Optional[str],
	    namespace: typing.List[str],
	    path: typing.Optional[Path],
	    element: Element,
	    group: Group,
	    conflicts: bool = False,
	) -> str:
		"""
        Insert a new element into the symbol map.
        Args:
                fqn: Full qualified name.
                path: Path associated with the element.
                element: Element to be registered.
                group: Group associated with the element, will be used for filtering.
                conflicts: Handle symbol FQN conflicts.
        """
		if name is None:
			# These are the unnamed elements that should be progpagated to other translation units.
			if Group.composition in group:
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
			originalElement = (self.getEntityResolved(fqn=fqn).assertValue(element=element).element)
			if not conflicts or element != originalElement:
				SymbolMap.errorSymbolConflict_(fqn, element, originalElement)

		self.map[fqn] = {
		    "g": group.value,
		    "p": path.as_posix() if path is not None else "",
		    "e": None,
		}

		# Resolve context
		context, _, _ = element.context.resolve()
		element.context = context

		# The element is also added to the entity map, to allow further modification that will
		# be written when serialize is called.
		self.entities[fqn] = elementToEntity(element=element)

		return fqn

	@staticmethod
	def errorSymbolConflict_(fqn: str, element1: Element, element2: Element) -> None:
		Error.handleFromElement(
		    element=element1,
		    message=f"Symbol name '{fqn}' is in conflict...",
		    throw=False,
		).extend(element=element2, message="...with this one.")

	def update(self, symbols: "SymbolMap") -> None:
		"""Register multiple symbols.

        Args:
                symbols: The symbol map.
        """
		for fqn, element in symbols.map.items():
			# Ignore private entries
			if FQN.isPrivate(fqn):
				continue
			existingElement = self._get(fqn=fqn)
			if existingElement is not None and element["p"] != existingElement["p"]:
				SymbolMap.errorSymbolConflict_(
				    fqn,
				    SymbolMap.metaToElement(element),
				    SymbolMap.metaToElement(existingElement),
				)
			self.map[fqn] = element

	def makeReference(self, fqn: str, group: typing.Optional[Group] = None) -> Element:
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

			entity.assertTrue(
			    condition=fqn in self.map,
			    message="Entry '{}' was not properly created before being added to the entities map.".format(fqn),
			)
			element = entity.element

			# Remove nested element and change them to references.
			if any([element.isNestedSequence(group.value) for group in NestedSequence]):
				preparedElement = element.copy(ignoreNested=[group.value for group in NestedSequence])
				for group in NestedSequence:
					nested = element.getNestedSequence(group.value)
					if nested is not None:
						sequence = SequenceBuilder()
						for nestedElement in nested:
							if nestedElement.isAttr("fqn"):
								fqnNested = nestedElement.getAttr("fqn").value
								# Remove private FQNs and keep them nested.
								# This is needed because when merging nested structure, some config
								# for example have unnamed elements and need to be copied with the rest.
								if FQN.isPrivate(fqnNested):
									removeFQNs.add(fqnNested)
									ElementBuilder.cast(nestedElement, ElementBuilder).removeAttr("fqn")
									sequence.pushBackElement(nestedElement)
								else:
									sequence.pushBackElement(self.makeReference(fqnNested))
							else:
								sequence.pushBackElement(nestedElement)
						if sequence:
							preparedElement.setNestedSequence(group.value, sequence)
				element = preparedElement

			# Serialize the element to the map.
			self.map[fqn]["e"] = element.serialize()

		# Remove some of the FQNs that have been copied.
		for fqn in removeFQNs:
			del self.map[fqn]

		# Sanity check to ensure that all entries in the map are valid.
		for fqn, entry in self.map.items():
			_ = Group(entry["g"])
			assert "e" in entry and entry["e"], f"Invalid element in the map: {entry}."
			assert "p" in entry, f"Missing path in the map: {entry}."

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

	def similarFQN(self, fqn: str) -> typing.List[str]:
		"""Find a related FQN that has similar name that the one in argument."""

		all_fqns = [k for k in [*self.map.keys(), *self.builtins.keys()] if not k.endswith("~")]
		similar = sortMatchingWeight(fqn, all_fqns)
		return similar if len(similar) <= 5 else [*similar[:5], "..."]

	def _terminateErrorMessageSimilarFQN(self, fqn: str) -> str:
		"""Create a string to be added to terminate an error message."""

		similar = self.similarFQN(fqn=fqn)
		if len(similar) == 1:
			return f", did you mean '{similar[0]}'?"
		elif len(similar) > 1:
			return f", did you mean:\n" + "\n".join([f"\t- {w}" for w in similar])
		return "."

	def getEntityResolved(self, fqn: str, exclude: typing.Optional[typing.List[Group]] = None) -> Result[EntityType]:
		"""
        Return an element from the symbol map.
        This call assumes that FQN is already resolved.
        """

		data = self._get(fqn=fqn, exclude=exclude)
		if data is None:
			message = f"Unable to find symbol '{fqn}'{self._terminateErrorMessageSimilarFQN(fqn=fqn)}"
			return Result.makeError(message)

		# Not memoized
		if fqn not in self.entities:
			element = SymbolMap.metaToElement(data)
			entity = elementToEntity(element=element)
			# Resolve dependencies
			for group in NestedSequence:
				if element.isNestedSequence(group.value):
					updatedSequence = SequenceBuilder()
					for nested in element.getNestedSequenceAssert(group.value):
						nestedEntity = elementToEntity(nested)
						# Resolve the reference if any
						if isinstance(nestedEntity, Reference):
							nestedEntity = self.getEntityResolved(fqn=nestedEntity.name,
							                                      exclude=exclude).assertValue(element=element)
						updatedSequence.pushBackElement(nestedEntity.element)
					ElementBuilder.cast(element, ElementBuilder).setNestedSequence(kind=group.value,
					                                                               sequence=updatedSequence)
			self.entities[fqn] = entity

		# Return the referenced type in case of reference.
		if isinstance(self.entities[fqn], Reference):
			return self.getEntityResolved(fqn=self.entities[fqn].name, exclude=exclude)

		# Return the entity.
		return Result[EntityType](self.entities[fqn])

	def getEntity(self, fqn: str, exclude: typing.Optional[typing.List[Group]] = None) -> Result[EntityType]:
		"""Return an element from the symbol map and resolves underlying types if needed."""

		namespaceChoices: typing.List[typing.List[str]] = [[]]
		result = None
		for name in FQN.toNamespace(fqn):
			for namespace in namespaceChoices:
				namespace.append(name)
				result = self.getEntityResolved(fqn=FQN.fromNamespace(namespace=namespace), exclude=exclude)
				if not result:
					continue
				if result.value.underlyingTypeFQN is not None:
					# If there is a type associated with this entity, add it to the namespace and its parents.
					namespaceChoices = [FQN.toNamespace(result.value.underlyingTypeFQN)]
					for fqn in self.getEntityResolved(result.value.underlyingTypeFQN).value.getParents():
						namespaceChoices.append(FQN.toNamespace(fqn))
				else:
					# Use the entity as a namespace for the following entries.
					namespaceChoices = [namespace]
				break

		if not result:
			return Result.makeError(f"Could not resolve symbol '{fqn}'.")

		return Result(result.value)

	def getChildren(self, fqn: str, groups: typing.Set[Group]) -> Result[typing.Dict[str, EntityType]]:
		"""Get all children of a specific element, the fqn passed into argument must already be resolved."""

		result = self.getEntityResolved(fqn=fqn)
		if not result:
			return Result.makeError(result.error)

		if result.value.underlyingTypeFQN is None:
			return Result.makeError(
			    f"The entity {result.value.fqn} does not have an underlying type, hence cannot have children.")

		children: typing.Dict[str, EntityType] = {}
		for prefix in [result.value.underlyingTypeFQN] + self.getEntityResolved(
		    result.value.underlyingTypeFQN).value.getParents():
			for fqn, item in self.items(groups=groups, startsWith=f"{prefix}.", depth=1):
				if fqn not in children:
					children[fqn] = item
		return Result(children)

	@staticmethod
	def fromSerialize(data: typing.Dict[str, typing.Any]) -> "SymbolMap":
		"""Create a symbol map from a serialized object."""

		symbols = SymbolMap()
		symbols.map = data
		return symbols

	def __repr__(self) -> str:
		content = []
		for key, data in {**self.map, **self.builtins}.items():
			content.append("[{}] {}".format(Group(data["g"]), key))
		return "\n".join(content)

import typing
from functools import cached_property
from dataclasses import dataclass

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

from tools.bdl.entities.impl.types import TypeCategory

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.visitors.symbol_map import Resolver
	from tools.bdl.entities.impl.fragment.type import Type
	from tools.bdl.entities.impl.entity import EntityExpression

@dataclass
class Data:
	# The name associated with this entry.
	name: typing.Optional[str]
	# The entity of this element.
	entity: "EntityExpression"
	# If this value is the default value, or one assigned during instanciation.
	default: bool = False
	# A number that corresponds to the order in which the parameter appears, given that
	# the collection is sorted by ascending order.
	order: int = -1

@dataclass
class Key:
	# The index related to this key.
	index: int
	# The name related to this key, if available.
	name: typing.Optional[str] = None
	def __str__(self) -> str:
		return str(self.index) if self.name is None else self.name

class ParametersCommon:

	def __init__(self, element: Element) -> None:
		self.element = element
		self.list: typing.List[Data] = []

	def __iter__(self) -> typing.Iterator["EntityExpression"]:
		for parameter in self.list:
			yield parameter.entity

	def __getitem__(self, index: int) -> "EntityExpression":
		return self.list[index].entity

	def __len__(self) -> int:
		return len(self.list)

	def __bool__(self) -> bool:
		return bool(self.list)

	def empty(self) -> bool:
		return not bool(self.list)

	def size(self) -> int:
		return len(self.list)

	def at(self, index: int) -> "EntityExpression":
		return self.list[index].entity

	@property
	def isNamed(self) -> typing.Optional[bool]:
		"""
		Wether or not a parameter pack contains named parameters or not.
		Returns None if it contains no parameters.
		"""
		if not self.empty():
			return self.at(0).isName
		return None

	@property
	def isVarArgs(self) -> typing.Optional[bool]:
		"""
		Wether or not the last parameter is a var args.
		"""
		if not self.empty():
			return self.at(-1).isVarArgs
		return None

	def append(self, entity: "EntityExpression", allowMix: bool = False, allowMultiVarArgs: bool = False, **kwargs: typing.Any) -> Data:
		entity.assertTrue(condition=not self.isVarArgs or allowMultiVarArgs,
			message=f"Variable arguments '{entity}' can only be present at the end of the parameter list.\n{str(self)}")
		self.list.append(Data(name=entity.name if entity.isName else None, entity=entity, **kwargs))
		# This is about having only named or only unamed parameters. Either:
		# (name1 = 0, name2 = 2, ...) or (0, 2, ...)
		entity.assertTrue(condition=allowMix or self.isNamed == entity.isName,
			message=f"Cannot mix named and unnamed parameters: 0:'{self.at(0)}' and {self.size()-1}:'{entity}':\nParameters:\n{str(self)}")
		return self.list[-1]

	def getUnderlyingTypeCategories(self, resolver: "Resolver") -> typing.Iterator[TypeCategory]:
		"""Get the typeCategory of the resolved type of the parameters."""

		for param in self:
			if param.isLiteral or param.isValue:
				pass
			elif param.isType:
				entityType = param.getEntityUnderlyingTypeResolved(resolver)
				entityType.assertTrue(condition=entityType.isRoleType, message="This entity must be of role type.")
				yield entityType.typeCategory  # type: ignore
			else:
				self.error(element=param, message="Unsupported parameter.")

	@staticmethod
	def makeKey(name: typing.Optional[str], index: int) -> Key:
		return index if name is None else name

	def items(self, includeVarArgs: bool) -> typing.Iterator[typing.Tuple[Key, "EntityExpression"]]:
		for key, entity, _ in self.itemsMetadata(includeVarArgs=includeVarArgs):
			yield key, entity

	def itemsMetadata(self, includeVarArgs: bool) -> typing.Iterator[typing.Tuple[Key, "EntityExpression", Data]]:
		for index, data in enumerate(self.list):
			if not data.entity.isVarArgs or includeVarArgs:
				yield Key(index=index, name=data.name), data.entity, data

	def keys(self, includeVarArgs: bool) -> typing.Iterator[Key]:
		for key, _, _ in self.itemsMetadata(includeVarArgs=includeVarArgs):
			yield key

	def getMetadata(self, key: Key) -> typing.Optional[Data]:
		"""Return the metadata at a specific key or None if not available"""

		searchByName = key.name is not None

		# Look for a match of the name if any.
		if searchByName:
			for k, _, metadata in self.itemsMetadata(includeVarArgs=True):
				if k.name == key.name:
					return metadata

		# Else check by index.
		if key.index < len(self.list):
			data = self.list[key.index]
			# Return the metadata only if the search by name was not done or
			# if the data doesn't have name (hence couldn't be matched in the search by name).
			if not searchByName or data.name is None:
				return self.list[key.index]
		return None

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this entity.
		"""
		dependencies = set()
		for param in self:
			dependencies.update(param.dependencies)
		return dependencies

	def __repr__(self) -> str:
		content = []
		for key, expression, metadata in self.itemsMetadata(includeVarArgs=True):
			content.append("{}: {}".format(str(key), str(metadata)))
		return "\n".join(content)

	def error(self, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> str:
		return Error.handleFromElement(element=self.element if element is None else element, message=message, throw=throw)

	def assertTrue(self, condition: bool, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> typing.Optional[str]:
		return Error.assertTrue(condition=condition, element=self.element if element is None else element, message=message, throw=throw)

ResolvedType = typing.Union[str, "Entity", "Type"]

class Parameters(ParametersCommon):
	"""
    Describes the parameter list, a collection of expression.
    """

	def __init__(self,
		element: Element,
		NestedElementType: typing.Type["EntityExpression"],
		nestedKind: typing.Optional[str] = None,
		filterFct: typing.Optional[typing.Callable[["EntityExpression"], bool]] = None) -> None:

		super().__init__(element=element)
		self.NestedElementType = NestedElementType

		# Fill the list with expressions from nestedKind.
		if nestedKind:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				for index, e in enumerate(sequence):
					from tools.bdl.entities.impl.entity import EntityExpression
					if filterFct is not None and not filterFct(EntityExpression(e)):
						continue
					# We must construct the NestedElementType only after filtering as
					# some entities are not constructible with any elements.
					self.append(self.NestedElementType(e), order=index)

	def copy(self) -> "Parameters":
		"""
		Copy the parameter list and optionally filter it.
		"""
		parameters = Parameters(element=self.element, NestedElementType=self.NestedElementType)
		parameters.list = self.list.copy()
		return parameters

	def resolve(self, resolver: "Resolver") -> None:
		"""Resolve all parameters."""

		for parameter in self:
			self.NestedElementType(parameter.element).resolveMemoized(resolver=resolver)

	def mergeDefaults(self, defaults: "Parameters") -> None:
		"""Merge default parameters with the current ones.
		Default parameters are always named and are ordered correctly.
		"""

		# Merge the values, do not include var args.
		for key, default, metadata in defaults.itemsMetadata(includeVarArgs=False):
			default.assertTrue(condition=default.isName,
				message=f"Default parameters must be named: '{default}'.")
			# Check if there is a match with the name first, if not check with the index.
			# If no match add the default parameter.
			maybeMetadata = self.getMetadata(key)
			if maybeMetadata is None:
				default.assertTrue(condition=not default.contracts.has("mandatory"),
					message=f"Missing mandatory parameter: '{str(key)}'.")
				maybeMetadata = self.append(default, allowMix=True, default=True)
			# Merge the metadata
			maybeMetadata.name = default.name
			maybeMetadata.order = key.index

	def itemsValuesOrTypes(self, resolver: "Resolver", varArgs: bool) -> typing.List[typing.Tuple[str, ResolvedType,
		Data]]:
		"""
        Iterate through the list and return values or types.
        """

		values: typing.List[typing.Tuple[str, ResolvedType, Data]] = []

		for key, expression, metadata in self.itemsMetadata(includeVarArgs=varArgs):

			if expression.literal is not None:
				values.append((str(key), expression.literal, metadata))

			elif expression.underlyingValueFQN is not None:
				entityValue: ResolvedType = resolver.getEntityResolved(fqn=expression.underlyingValueFQN).assertValue(
					element=expression.element)
				entityValue.assertTrue(condition=entityValue.category == "expression", message=f"A value FQN must resolve into an expression, not {expression}")

				# If these are default arguments, use the default value.
				#if metadata.default and value.isName:
					# Create an unamed value
				#	element = ElementBuilder.cast(value.element.copy(), ElementBuilder).removeAttr(key="name").get()
				#	value = Expression(element)

				values.append((str(key), entityValue, metadata))

			# Temporary values, for example: arg1 = Integer(12)
			elif expression.isParameters:
				values.append((str(key), expression, metadata))

			# Types or variables, for example: arg1 = core
			elif expression.isType:
				values.append((str(key), expression.type, metadata))

		return values

	def getValuesOrTypesAsDict(self, resolver: "Resolver", varArgs: bool) -> typing.Dict[str, ResolvedType]:
		"""
        Get the values as a dictionary.
        """
		values = self.itemsValuesOrTypes(resolver=resolver, varArgs=varArgs)
		return {entry[0]: entry[1] for entry in values}

	def toResolvedSequence(self, resolver: "Resolver", varArgs: bool) -> Sequence:
		"""
		Build the resolved sequence of those parameters.
		Must be called after mergeDefaults.
		"""
		items = self.itemsValuesOrTypes(resolver=resolver, varArgs=varArgs)
		if self.isNamed:
			items = sorted(items, key=lambda k: k[2].order)

		# Build the sequence
		def buildSequence(entries) -> Sequence:
			from tools.bdl.entities.impl.fragment.type import Type

			sequence = SequenceBuilder()
			for key, item, metadata in entries:
				if isinstance(item, str):
					element = ElementBuilder().setAttr(key="value", value=item).setAttr(key="literal", value=item)
				else:
					element = item.element.copy()
				if self.isNamed:
					ElementBuilder.cast(element, ElementBuilder).setAttr(key="key", value=key)
				sequence.pushBackElement(element)
			return sequence

		return buildSequence(items)


class ResolvedParameters(ParametersCommon):

	def __init__(self, element: Element, nestedKind: typing.Optional[str], allowMultiVarArgs: bool = False) -> None:

		super().__init__(element=element)

		if nestedKind is not None:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from tools.bdl.entities.impl.expression import Expression
				for index, e in enumerate(sequence):
					self.append(Expression(e), allowMultiVarArgs=allowMultiVarArgs)

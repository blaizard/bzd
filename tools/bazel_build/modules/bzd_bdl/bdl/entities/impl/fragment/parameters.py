import typing
import math
from functools import cached_property
from dataclasses import dataclass

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error, AssertionResult

from bdl.contracts.validation import Validation, SchemaDict
from bdl.entities.impl.types import Category

if typing.TYPE_CHECKING:
	from bdl.entities.impl.expression import Expression
	from bdl.visitors.symbol_map import Resolver
	from bdl.entities.impl.fragment.symbol import Symbol
	from bdl.entities.impl.entity import EntityExpression


@dataclass
class Data:
	# The name associated with this entry.
	name: typing.Optional[str]
	# The entity of this element.
	entity: "EntityExpression"
	# If this value is the default value, or one assigned during instantiation.
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
	def names(self) -> typing.Set[str]:
		"""Retrieve the set of names of the various elements from the list."""

		return {data.name for data in self.list if data.name is not None}

	@property
	def isNamed(self) -> typing.Optional[bool]:
		"""
        Whether or not a parameter pack contains named parameters or not.
        Returns None if it contains no parameters.
        """
		if not self.empty():
			return self.at(0).isName
		return None

	@property
	def isVarArgs(self) -> typing.Optional[bool]:
		"""
        Whether or not the last parameter is a var args.
        """
		if not self.empty():
			return self.at(-1).isVarArgs
		return None

	def append(
	    self,
	    entity: "EntityExpression",
	    allowMix: bool = False,
	    allowMultiVarArgs: bool = False,
	    **kwargs: typing.Any,
	) -> Data:
		entity.assertTrue(
		    condition=not self.isVarArgs or allowMultiVarArgs,
		    message=f"Variable arguments '{entity}' can only be present at the end of the parameter list.\n{str(self)}",
		)
		name = entity.name if entity.isName else None
		entity.assertTrue(
		    condition=(name is None) or (name not in self.names),
		    message=f"The name of the parameter '{name}' is duplicated.",
		)
		# Add the element.
		self.list.append(Data(name=name, entity=entity, **kwargs))
		# This is about having only named or only unnamed parameters. Either:
		# (name1 = 0, name2 = 2, ...) or (0, 2, ...)
		entity.assertTrue(
		    condition=allowMix or self.isNamed == entity.isName,
		    message=
		    f"Cannot mix named and unnamed parameters: 0:'{self.at(0)}' and {self.size()-1}:'{entity}':\nParameters:\n{str(self)}",
		)
		return self.list[-1]

	def getUnderlyingTypeCategories(self, resolver: "Resolver") -> typing.Iterator[Category]:
		"""Get the category of the resolved type of the parameters."""

		for param in self:
			if param.isLiteral or param.isValue or param.isRegexpr:
				pass
			elif param.isSymbol:
				entityType = param.getEntityUnderlyingTypeResolved(resolver)
				entityType.assertTrue(
				    condition=entityType.isRoleType,
				    message="This entity must be of role type.",
				)
				yield entityType.category
			else:
				self.error(element=param.element, message="Unsupported parameter.")

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

	def getOrders(self) -> typing.Tuple[int, int]:
		"""Get the min and max of the order field of the data."""

		if len(self.list) == 0:
			return 0, 0
		retMin = math.inf
		retMax = -math.inf
		for _, data in enumerate(self.list):
			retMin = min(retMin, data.order)
			retMax = max(retMax, data.order)
		return int(retMin), int(retMax)

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
		"""Output the dependency list for this entity."""

		dependencies = set()
		for param in self:
			dependencies.update(param.dependencies)
		return dependencies

	def makeValidation(self, resolver: "Resolver", forTemplate: bool) -> Validation[SchemaDict]:
		"""Generate the validation object."""

		schema: typing.Dict[str, str] = {}
		for key, expression in self.items(includeVarArgs=True):
			keyStr = ("*" if expression.isVarArgs else str(key.index if forTemplate else key))
			maybeContracts = (expression.contracts.validationForTemplate
			                  if forTemplate else expression.contracts.validationForValue)
			schema[keyStr] = maybeContracts if maybeContracts is not None else ""
			# Add that template argument must be part of the given type.
			if forTemplate:
				expression.assertTrue(
				    condition=expression.underlyingTypeFQN is not None,
				    message=f"The type '{expression}' was not resolved.",
				)
				schema[keyStr] += f" convertible({str(expression.underlyingTypeFQN)})"

		if schema:
			try:
				return Validation(schema=schema, args={"resolver": resolver})
			except Exception as e:
				self.error(message=str(e))

		return Validation(schema={}, args={"resolver": resolver})

	def makeValidationForTemplate(self, resolver: "Resolver") -> Validation[SchemaDict]:
		"""Generate the validation object for template parameters."""

		return self.makeValidation(resolver=resolver, forTemplate=True)

	def makeValidationForValues(self, resolver: "Resolver") -> Validation[SchemaDict]:
		"""Generate the validation object for value parameters."""

		return self.makeValidation(resolver=resolver, forTemplate=False)

	def __repr__(self) -> str:
		content = []
		for key, expression, metadata in self.itemsMetadata(includeVarArgs=True):
			content.append("{}: {}".format(str(key), str(metadata)))
		return "\n".join(content)

	def error(self, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> AssertionResult:
		return Error.handleFromElement(
		    element=self.element if element is None else element,
		    message=message,
		    throw=throw,
		)

	def assertTrue(
	    self,
	    condition: bool,
	    message: str,
	    element: typing.Optional[Element] = None,
	    throw: bool = True,
	) -> AssertionResult:
		return Error.assertTrue(
		    condition=condition,
		    element=self.element if element is None else element,
		    message=message,
		    throw=throw,
		)


ResolvedType = typing.Union["EntityExpression", "Symbol"]


class Parameters(ParametersCommon):
	"""
    Describes the parameter list, a collection of expression.
    """

	def __init__(
	    self,
	    element: Element,
	    NestedElementType: typing.Type["EntityExpression"],
	    nestedKind: typing.Optional[str] = None,
	    filterFct: typing.Optional[typing.Callable[["EntityExpression"], bool]] = None,
	) -> None:
		super().__init__(element=element)
		self.NestedElementType = NestedElementType

		# Fill the list with expressions from nestedKind.
		if nestedKind:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from bdl.entities.impl.entity import EntityExpression

				for index, e in enumerate(sequence):
					if filterFct is not None and not filterFct(EntityExpression(e)):
						continue
					# We must construct the NestedElementType only after filtering as
					# some entities are not constructible with any elements.
					self.append(self.NestedElementType(e), order=index)

	def extend(self, params: "Parameters", left: bool = False) -> None:
		"""Merge another params into this one."""

		retMin, retMax = self.getOrders()
		order = (retMin - len(params)) if left else (retMax + 1)
		for expression in params:
			self.append(expression, order=order)
			order += 1

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
			default.assertTrue(
			    condition=default.isName,
			    message=f"Default parameters must be named: '{default}'.",
			)
			# Check if there is a match with the name first, if not check with the index.
			# If no match add the default parameter.
			maybeMetadata = self.getMetadata(key)
			if maybeMetadata is None:
				default.assertTrue(
				    condition=not default.contracts.has("mandatory"),
				    message=f"Missing mandatory parameter: '{str(key)}'.",
				)
				maybeMetadata = self.append(default, allowMix=True, default=True)
			# Merge the metadata
			maybeMetadata.name = default.name
			maybeMetadata.order = key.index

	def getValuesOrTypesAsDict(self, resolver: "Resolver", varArgs: bool) -> typing.Dict[str, ResolvedType]:
		"""
        Get the values as a dictionary.
        """
		return {str(key): expression for key, expression, _ in self.itemsMetadata(includeVarArgs=varArgs)}

	def toResolvedSequence(self, resolver: "Resolver", varArgs: bool) -> Sequence:
		"""
        Build the resolved sequence of those parameters.
        Must be called after mergeDefaults.
        """
		items = [metadata for _, _, metadata in self.itemsMetadata(includeVarArgs=varArgs)]
		items = sorted(items, key=lambda k: k.order)

		sequence = SequenceBuilder()
		for metadata in items:
			sequence.pushBackElement(metadata.entity.element)

		return sequence

	def makeParametersResolved(self, name: str, resolver: "Resolver", expected: "Parameters") -> None:
		"""Create and resolve the parametersResolved given expected parameters."""

		# Merge its default values
		self.mergeDefaults(defaults=expected)

		# Save the resolved parameters before the validation is completed. This is because
		# it might make use of the parametersResolved.
		sequenceValues = self.toResolvedSequence(resolver=resolver, varArgs=False)
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence(f"{name}_resolved", sequenceValues)

		sequence = expected.toResolvedSequence(resolver=resolver, varArgs=True)
		sequence += ([sequence[-1]] * (len(sequenceValues) - len(sequence)) if expected.isVarArgs else [])
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence(f"{name}_expected", sequence)

import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.visitors.symbol_map import SymbolMap
	from tools.bdl.entities.impl.fragment.type import Type
	from tools.bdl.entities.impl.entity import Entity

ResolvedType = typing.Union[str, "Entity", "Type"]


class ParametersCommon:

	def __init__(self, element: Element) -> None:
		self.element = element
		self.list: typing.List["Expression"] = []

	def __iter__(self) -> typing.Iterator["Expression"]:
		for parameter in self.list:
			yield parameter

	def __getitem__(self, index: int) -> "Expression":
		return self.list[index]

	def __len__(self) -> int:
		return len(self.list)

	def __bool__(self) -> bool:
		return bool(self.list)

	def empty(self) -> bool:
		return not bool(self.list)

	def size(self) -> int:
		return len(self.list)

	@staticmethod
	def makeKey(entity: "Expression", index: int) -> str:
		return entity.name if entity.isName else str(index)

	def items(self) -> typing.Iterator[typing.Tuple[str, "Expression"]]:
		for index, entity in enumerate(self.list):
			name = ParametersCommon.makeKey(entity, index)
			yield name, entity

	def contains(self, name: str) -> bool:
		"""
		Wether or not a named parameters contains the entry.
		"""
		for key, _ in self.items():
			if key == name:
				return True
		return False


class Parameters(ParametersCommon):
	"""
    Describes the parameter list, a collection of expression.
    """

	def __init__(self,
		element: Element,
		nestedKind: typing.Optional[str] = None,
		filterFct: typing.Optional[typing.Callable[["Expression"], bool]] = None) -> None:

		super().__init__(element=element)

		# Sorted list of keys from defaults
		self.keysSorted: typing.Optional[typing.List[str]] = None
		# Keys of default values
		self.defaults: typing.Set[str] = set()

		if nestedKind:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from tools.bdl.entities.impl.expression import Expression
				for index, e in enumerate(sequence):
					expression = Expression(e)
					if filterFct is not None and not filterFct(expression):
						continue
					self.list.append(expression)
					expression.assertTrue(condition=self.isNamed == expression.isName,
						message="Cannot mix named and unnamed parameters: '{}' and '{}'.".format(
						self.list[0], expression))
					expression.assertTrue(condition=not expression.isVarArgs or index == len(sequence) - 1,
						message="Variable arguments can only be present at the end of the parameter list.")

		# Sanity check.
		if self.list and self.isNamed:
			assert not self.isVarArgs, "Cannot have named and varargs."

	@property
	def isNamed(self) -> typing.Optional[bool]:
		"""
		Wether or not a parameter pack contains named parameters or not.
		Returns None if it contains no parameters.
		"""
		if self.list:
			return self.list[0].isName
		return None

	@property
	def isVarArgs(self) -> typing.Optional[bool]:
		"""
		Wether or not the last parameter is a var args.
		"""
		if self.list:
			return self.list[-1].isVarArgs
		return None

	def mergeDefaults(self, parameters: "Parameters") -> None:
		"""
		Merge default parameters with this.
		"""
		if not bool(parameters):
			return
		if bool(self):
			if self.isNamed != parameters.isNamed:
				Error.assertTrue(element=self.element,
					condition=bool(self.isNamed),
					message="Requires named parameters.")
				Error.assertTrue(element=self.element,
					condition=not bool(self.isNamed),
					message="Requires unnamed parameters.")

		# Merge
		if parameters.isNamed:
			self.keysSorted = []
			for name, expression in parameters.items():
				self.keysSorted.append(name)
				if not self.contains(name) and not expression.contracts.has("mandatory"):
					self.defaults.add(ParametersCommon.makeKey(expression, len(self.list)))
					self.list.append(expression)

		elif len(self) < len(parameters):
			Error.assertTrue(element=self.element,
				condition=not self.isVarArgs,
				message="Variable arguments must be at the end.")
			hasSkipped = False
			for expression in parameters.list[len(self.list):]:
				if not expression.contracts.has("mandatory"):
					Error.assertTrue(element=self.element,
						condition=not hasSkipped,
						message="Mandatory positional parameters must be at the end.")
					self.defaults.add(ParametersCommon.makeKey(expression, len(self.list)))
					self.list.append(expression)
				else:
					hasSkipped = True

	def resolve(self,
		symbols: "SymbolMap",
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
        Resolve all parameters.
        """

		for parameter in self:
			parameter.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

	def itemsValuesOrTypes(self, symbols: "SymbolMap",
		exclude: typing.Optional[typing.List[str]]) -> typing.List[typing.Tuple[str, ResolvedType]]:
		"""
        Iterate through the list and return values or types.
        """

		values: typing.List[typing.Tuple[str, ResolvedType]] = []

		for key, expression in self.items():

			if expression.literal is not None:
				values.append((key, expression.literal))

			elif expression.underlyingValue is not None:
				value: ResolvedType = symbols.getEntityResolved(fqn=expression.underlyingValue).assertValue(
					element=expression.element)

				# If these are default arguments, use the default value.
				if key in self.defaults:
					from tools.bdl.entities.impl.expression import Expression
					assert isinstance(value, Expression)
					if value.literal:
						# I beleive that this code path is never used.
						value = value.literal
					elif value.isName:
						# Create an unamed value
						element = ElementBuilder.cast(value.element.copy(), ElementBuilder).removeAttr(key="name")
						value = Expression(element)

				values.append((key, value))

			else:
				values.append((key, expression.type))

		return values

	def getValuesOrTypesAsDict(self, symbols: "SymbolMap", exclude: typing.Optional[typing.List[str]]) -> typing.Dict[
		str, ResolvedType]:
		"""
        Get the values as a dictionary.
        """
		values = self.itemsValuesOrTypes(symbols=symbols, exclude=exclude)
		return {entry[0]: entry[1] for entry in values}

	def getValuesOrTypesAsList(self, symbols: "SymbolMap",
		exclude: typing.Optional[typing.List[str]]) -> typing.List[ResolvedType]:
		"""
        Get the values as a list.
        """
		values = self.itemsValuesOrTypes(symbols=symbols, exclude=exclude)
		return [entry[1] for entry in values]

	def toResolvedSequence(self, symbols: "SymbolMap", exclude: typing.Optional[typing.List[str]]) -> Sequence:
		"""
		Build the resolved sequence of those parameters.
		Must be called after mergeDefaults.
		"""
		sequence = SequenceBuilder()

		if self.isNamed:
			itemsDict = self.getValuesOrTypesAsDict(symbols=symbols, exclude=exclude)
			items = [itemsDict[name] for name in self.keysSorted] if self.keysSorted else []
		else:
			items = self.getValuesOrTypesAsList(symbols=symbols, exclude=exclude)

		# Build the sequence
		for item in items:
			if isinstance(item, str):
				sequence.pushBackElement(ElementBuilder().setAttr(key="value", value=item))
			else:
				sequence.pushBackElement(item.element)

		return sequence


class ResolvedParameters(ParametersCommon):

	def __init__(self, element: Element, nestedKind: str) -> None:

		super().__init__(element=element)

		sequence = self.element.getNestedSequence(nestedKind)
		if sequence:
			from tools.bdl.entities.impl.expression import Expression
			self.list = [Expression(e) for index, e in enumerate(sequence)]

	@property
	def isNamed(self) -> typing.Optional[bool]:
		return False

	@property
	def isVarArgs(self) -> typing.Optional[bool]:
		return False

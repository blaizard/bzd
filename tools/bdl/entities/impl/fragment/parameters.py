import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.visitors.symbol_map import SymbolMap
	from tools.bdl.entities.impl.fragment.type import Type
	from tools.bdl.entities.impl.entity import Entity

ResolvedType = typing.Union[str, "Entity", "Type"]


class Parameters:
	"""
    Describes the parameter list, a collection of expression.
    """

	def __init__(self,
		element: Element,
		nestedKind: typing.Optional[str] = None,
		filterFct: typing.Optional[typing.Callable[["Expression"], bool]] = None) -> None:
		self.element = element

		# Build the parameter list
		self.list: typing.List["Expression"] = []
		if nestedKind:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from tools.bdl.entities.impl.expression import Expression
				for e in sequence:
					expression = Expression(e)
					if filterFct is not None and not filterFct(expression):
						continue
					self.list.append(expression)
					Error.assertTrue(element=self.element,
						condition=self.isNamed == expression.isName,
						message="Cannot mix named and unnamed parameters.")

	def __iter__(self) -> typing.Iterator["Expression"]:
		for parameter in self.list:
			yield parameter

	def __getitem__(self, index: int) -> "Expression":
		return self.list[index]

	def __len__(self) -> int:
		return len(self.list)

	def __bool__(self) -> bool:
		return bool(self.list)

	@property
	def isNamed(self) -> typing.Optional[bool]:
		"""
		Wether or not a parameter pack contains named parameters or not.
		Returns None if it contains no parameters.
		"""
		if self.list:
			return self.list[0].isName
		return None

	def contains(self, name: str) -> bool:
		"""
		Wether or not a named parameters contains the entry.
		"""
		for key, _ in self.items():
			if key == name:
				return True
		return False

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
			for name, expression in parameters.items():
				if not self.contains(name) and not expression.contracts.has("mandatory"):
					self.list.append(expression)

		elif len(self) < len(parameters):
			hasSkipped = False
			for expression in parameters.list[len(self.list):]:
				if not expression.contracts.has("mandatory"):
					Error.assertTrue(element=self.element,
						condition=not hasSkipped,
						message="Mandatory positional parameters must be at the end.")
					self.list.append(expression)
				else:
					hasSkipped = True

	def empty(self) -> bool:
		return not bool(self.list)

	def size(self) -> int:
		return len(self.list)

	def items(self) -> typing.Iterator[typing.Tuple[str, "Expression"]]:
		for index, entity in enumerate(self.list):
			name = entity.name if entity.isName else str(index)
			yield name, entity

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
				value = symbols.getEntityResolved(fqn=expression.underlyingValue).assertValue(
					element=expression.element)
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

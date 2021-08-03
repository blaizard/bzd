import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.visitors.preprocess.symbol_map import SymbolMap
	from tools.bdl.entities.impl.fragment.type import Type
	from tools.bdl.entities.impl.entity import Entity

ResolvedType = typing.Union[str, "Entity", "Type"]


class Parameters:
	"""
    Describes the parameter list, a collection of expression.
    """

	def __init__(self, element: Element, nestedKind: typing.Optional[str]) -> None:
		self.element = element

		# Build the parameter list
		self.list = []
		if nestedKind:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from tools.bdl.entities.impl.expression import Expression
				for e in sequence:
					self.list.append(Expression(e))

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

	def iterate(
		self, filterFct: typing.Optional[typing.Callable[["Entity"],
		bool]]) -> typing.Iterator[typing.Tuple[str, "Expression"]]:
		isNamed: typing.Optional[bool] = None
		for index, entity in enumerate(self.list):
			if filterFct is not None and not filterFct(entity):
				continue
			if isNamed is not None:
				Error.assertTrue(element=self.element,
					condition=isNamed == entity.isName,
					message="Cannot mix named and unmaed parameters.")
			else:
				isNamed = entity.isName
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

	def iterateValuesOrTypes(
		self,
		symbols: "SymbolMap",
		exclude: typing.Optional[typing.List[str]],
		filterFct: typing.Optional[typing.Callable[["Entity"], bool]] = None
	) -> typing.List[typing.Tuple[str, ResolvedType]]:
		"""
        Iterate through the list and return values or types.
        """

		values: typing.List[typing.Tuple[str, ResolvedType]] = []

		for key, entity in self.iterate(filterFct=filterFct):

			if entity.literal is not None:
				values.append((key, entity.literal))

			elif entity.underlyingValue is not None:
				value = symbols.getEntityAssert(fqn=entity.underlyingValue, element=entity.element)
				values.append((key, value))

			else:
				values.append((key, entity.type))

		return values

	def getValuesOrTypesAsDict(
			self,
			symbols: "SymbolMap",
			exclude: typing.Optional[typing.List[str]],
			filterFct: typing.Optional[typing.Callable[["Entity"], bool]] = None) -> typing.Dict[str, ResolvedType]:
		"""
        Get the values as a dictionary.
        """
		values = self.iterateValuesOrTypes(symbols=symbols, exclude=exclude, filterFct=filterFct)
		return {entry[0]: entry[1] for entry in values}

	def getValuesOrTypesAsList(
			self,
			symbols: "SymbolMap",
			exclude: typing.Optional[typing.List[str]],
			filterFct: typing.Optional[typing.Callable[["Entity"], bool]] = None) -> typing.List[ResolvedType]:
		"""
        Get the values as a list.
        """
		values = self.iterateValuesOrTypes(symbols=symbols, exclude=exclude, filterFct=filterFct)
		return [entry[1] for entry in values]

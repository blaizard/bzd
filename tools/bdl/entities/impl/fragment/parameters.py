import typing
from functools import cached_property

from bzd.parser.element import Element

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
				self.list = [Expression(elementParameter) for elementParameter in sequence]

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

	def iterate(self) -> typing.Iterator[typing.Tuple[str, "Expression"]]:
		for index, parameter in enumerate(self.list):
			name = parameter.name if parameter.isName else str(index)
			yield name, parameter

	def resolve(self,
		symbols: "SymbolMap",
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
        Resolve all parameters.
        """

		for parameter in self:
			parameter.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

	def getValuesOrTypesAsDict(self, symbols: "SymbolMap", exclude: typing.Optional[typing.List[str]]) -> typing.Dict[
		str, ResolvedType]:
		"""
        Get the values as a dictionary.
        """
		valuesAsList = self.getValuesOrTypesAsList(symbols=symbols, exclude=exclude)
		values = {}
		index = 0
		for name, entity in self.iterate():
			values[name] = valuesAsList[index]
			index += 1

		return values

	def getValuesOrTypesAsList(self, symbols: "SymbolMap",
		exclude: typing.Optional[typing.List[str]]) -> typing.List[ResolvedType]:
		"""
        Get the values as a list.
        """
		values: typing.List[ResolvedType] = []
		for entity in self:
			if entity.literal is not None:
				values.append(entity.literal)

			elif entity.underlyingValue is not None:
				value = symbols.getEntityAssert(fqn=entity.underlyingValue, element=entity.element)
				values.append(value)

			else:
				values.append(entity.type)

		return values

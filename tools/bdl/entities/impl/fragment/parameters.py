import typing
from functools import cached_property

from bzd.parser.element import Element

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.visitors.preprocess.symbol_map import SymbolMap


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

		for name, parameter in self.iterate():
			if parameter.isValue:
				pass

			else:
				assert parameter.isType
				parameter.type.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
				# TODO: resolve underlying value

	@cached_property
	def valuesAsDict(self) -> typing.Dict[str, str]:
		"""
        Get the values as a dictionary.
        """
		values = {}
		for name, parameter in self.iterate():
			if parameter.isValue:
				values[name] = parameter.value

			else:
				assert parameter.isType
				values[name] = parameter.type.kind

		return values

	@cached_property
	def valuesAsList(self) -> typing.List[str]:
		"""
        Get the values as a list.
        """
		values = []
		for parameter in self:
			if parameter.isValue:
				values.append(parameter.value)

			else:
				assert parameter.isType
				values.append(parameter.type.kind)

		return values

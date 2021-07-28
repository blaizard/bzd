import typing
from functools import cached_property

from bzd.parser.element import Element

from tools.bdl.entities.impl.fragment.type import Type


class Parameter:
	"""
    Describes a single parameter.
    """

	def __init__(self, element: Element) -> None:
		self.element = element

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name")

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", template="template")

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def value(self) -> str:
		return self.element.getAttr("value").value

	@property
	def raw(self) -> str:
		return self.value if self.isValue else self.type.kind


class Parameters:
	"""
    Describes the whole parameter list, a collection of parameter.
    """

	def __init__(self, element: Element, nestedKind: str) -> None:
		self.element = element

		# Build the parameter list
		self.list = []
		sequence = self.element.getNestedSequence(nestedKind)
		if sequence:
			self.list = [Parameter(elementParameter) for elementParameter in sequence]

	def __iter__(self) -> typing.Iterator[Parameter]:
		for parameter in self.list:
			yield parameter

	def __getitem__(self, index: int) -> Parameter:
		return self.list[index]

	def __len__(self) -> int:
		return len(self.list)

	def __bool__(self) -> bool:
		return bool(self.list)

	def empty(self) -> bool:
		return not bool(self.list)

	def iterate(self) -> typing.Iterator[typing.Tuple[str, Parameter]]:
		for index, parameter in enumerate(self.list):
			name = parameter.name if parameter.isName else str(index)
			yield name, parameter

	def resolve(self,
		symbols: typing.Any,
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
	def values(self) -> typing.Dict[str, str]:
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

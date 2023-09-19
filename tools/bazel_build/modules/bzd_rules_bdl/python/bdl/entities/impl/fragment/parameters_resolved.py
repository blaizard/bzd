import typing

from bzd.parser.element import Element
from bzd.parser.error import Error, AssertionResult

if typing.TYPE_CHECKING:
	from bdl.entities.impl.entity import EntityExpression
	from bdl.entities.impl.fragment.symbol import Symbol


class ParametersResolvedItem:
	"""Entry contained in the resolved parameters list."""

	def __init__(self, param: "EntityExpression", expected: "EntityExpression") -> None:
		self.param = param
		self.expected = expected

	@property
	def isName(self) -> bool:
		return self.expected.isName

	@property
	def name(self) -> str:
		return self.expected.name

	@property
	def symbol(self) -> "Symbol":
		return self.expected.symbol

	@property
	def sameType(self) -> bool:
		"""If the expected and param types are the same."""
		if self.expected.isSymbol and self.param.isSymbol:
			return self.expected.symbol == self.param.symbol
		return False

	@property
	def comment(self) -> typing.Optional[str]:
		return (self.expected.comment if self.param.comment is None else self.param.comment)

	@property
	def isRValue(self) -> bool:
		return self.param.isRValue

	@property
	def isLValue(self) -> bool:
		return self.param.isLValue

	def error(self, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> AssertionResult:
		return Error.handleFromElement(
		    element=self.param.element if element is None else element,
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
		    element=self.param.element if element is None else element,
		    message=message,
		    throw=throw,
		)


class ParametersResolved:

	def __init__(
	    self,
	    element: Element,
	    NestedElementType: typing.Type["EntityExpression"],
	    param: str,
	    expected: str,
	) -> None:
		"""Construct the ParametersResolved object.

        Args:
                element: The element associated with these resolved parameters.
                param: The name of the nested sequence containing the resolved parameters.
                expected: The name of the nested sequence containing the expected parameters.
        """
		self.element = element
		self.list: typing.List[ParametersResolvedItem] = []

		paramSequence = self.element.getNestedSequenceOrEmpty(param)
		expectedSequence = self.element.getNestedSequenceOrEmpty(expected)
		for paramElement, expectedElement in zip(paramSequence, expectedSequence):
			self.list.append(ParametersResolvedItem(NestedElementType(paramElement),
			                                        NestedElementType(expectedElement)))

	@property
	def dependencies(self) -> typing.Set[str]:
		"""Output the dependency list for this entity."""

		dependencies = set()
		for param in self:
			dependencies.update(param.param.dependencies)
		return dependencies

	def __iter__(self) -> typing.Iterator[ParametersResolvedItem]:
		for parameter in self.list:
			yield parameter

	def __getitem__(self, index: int) -> ParametersResolvedItem:
		return self.list[index]

	def __len__(self) -> int:
		return len(self.list)

	def __bool__(self) -> bool:
		return bool(self.list)

	def empty(self) -> bool:
		return not bool(self.list)

	def size(self) -> int:
		return len(self.list)

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

	def __repr__(self) -> str:
		content = []
		for item in self.list:
			content.append(f"{item.name}: {item.param}, expected={item.expected}")
		return "\n".join(content)

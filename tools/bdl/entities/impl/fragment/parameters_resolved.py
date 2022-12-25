import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.entity import EntityExpression
	from tools.bdl.entities.impl.fragment.type import Type

class ParametersResolvedItem:
	"""Entry contained in the resolved parameters list."""

	def __init__(self, param: "EntityExpression", expected: "EntityExpression") -> None:
		self.param =  param
		self.expected =  expected

		# All expected element must have a name.
		Error.assertHasAttr(element=self.expected.element, attr="name")

	@property
	def name(self) -> str:
		return self.expected.name

	@property
	def type(self) -> "Type":
		return self.expected.type

	@property
	def comment(self) -> typing.Optional[str]:
		return self.expected.comment if self.param.comment is None else self.param.comment

	@property
	def isRValue(self) -> bool:
		return self.param.isRValue

	@property
	def isLValue(self) -> bool:
		return self.param.isLValue

class ParametersResolved:

	def __init__(self, element: Element, NestedElementType: typing.Type["EntityExpression"], param: str, expected: str) -> None:
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
		for (paramElement, expectedElement) in zip(paramSequence, expectedSequence):
			self.list.append(ParametersResolvedItem(NestedElementType(paramElement), NestedElementType(expectedElement)))

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

	def error(self, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> str:
		return Error.handleFromElement(element=self.element if element is None else element, message=message, throw=throw)

	def assertTrue(self, condition: bool, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> typing.Optional[str]:
		return Error.assertTrue(condition=condition, element=self.element if element is None else element, message=message, throw=throw)

	def __repr__(self) -> str:
		content = []
		for item in self.list:
			content.append(f"{item.name}: {item.param}, expected={item.expected}")
		return "\n".join(content)

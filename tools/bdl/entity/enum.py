import typing

from bzd.parser.element import Element, Sequence
from bzd.parser.error import assertHasSequence, assertHasAttr
from bzd.parser.visitor import Visitor as VisitorBase


class _Visitor(VisitorBase[str, typing.List[str]]):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		assertHasAttr(element=element, attr="name")
		result.append(element.getAttr("name").value)
		return result


class Enum:

	def __init__(self, element: Element) -> None:

		assertHasAttr(element=element, attr="name")
		assertHasSequence(element=element, sequence="values")
		self.element = element

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def category(self) -> str:
		return "enum"

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def valueList(self) -> typing.List[str]:
		sequence = self.element.getNestedSequence("values")
		assert sequence is not None
		return _Visitor().visit(sequence=sequence)

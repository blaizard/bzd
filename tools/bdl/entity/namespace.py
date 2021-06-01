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


class Namespace:

	def __init__(self, element: Element) -> None:

		assertHasSequence(element=element, sequence="name")
		self.element = element

	@property
	def symbol(self) -> str:
		return self.nameList[-1]

	@property
	def category(self) -> str:
		return "namespace"

	@property
	def nameList(self) -> typing.List[str]:
		sequence = self.element.getNestedSequence("name")
		assert sequence is not None
		return _Visitor().visit(sequence=sequence)

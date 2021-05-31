import typing

from bzd.parser.element import Element
from bzd.parser.error import assertHasAttr
from tools.bdl.entity.type import Type
from tools.bdl.entity.contract import Contracts


class Variable:

	def __init__(self, element: Element, visitor: typing.Any) -> None:

		assertHasAttr(element=element, attr="name")
		assertHasAttr(element=element, attr="type")

		self.element = element
		self.visitor = visitor

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def const(self) -> bool:
		return self.element.isAttr("const")

	@property
	def type(self) -> Type:
		return Type(element=self.element)

	@property
	def value(self) -> typing.Optional[str]:
		return self.element.getAttrValue("value")

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.visitor.visitComment(self.element.getAttrValue("comment"))

	@property
	def contracts(self) -> Contracts:
		return Contracts(sequence=self.element.getNestedSequence("contract"), visitor=self.visitor)

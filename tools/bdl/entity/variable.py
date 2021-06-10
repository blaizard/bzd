import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entity.fragment.type import Type
from tools.bdl.entity.fragment.contract import Contracts
from tools.bdl.entity.entity import Entity


class Variable(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)
		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="type")

	@property
	def category(self) -> str:
		return "variable"

	@property
	def const(self) -> bool:
		return self.element.isAttr("const")

	@property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", template="template")

	@property
	def value(self) -> typing.Optional[str]:
		return self.element.getAttrValue("value")

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def contracts(self) -> Contracts:
		return Contracts(sequence=self.element.getNestedSequence("contract"))

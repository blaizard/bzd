import typing

from bzd.parser.element import Element
from bzd.parser.error import Error
from tools.bdl.entity.type import Type
from tools.bdl.entity.contract import Contracts


class Variable:

	def __init__(self, element: Element) -> None:

		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="type")

		self.element = element

	@property
	def category(self) -> str:
		return "variable"

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
		return self.element.getAttrValue("comment")

	@property
	def contracts(self) -> Contracts:
		return Contracts(sequence=self.element.getNestedSequence("contract"))

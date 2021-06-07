import typing

from bzd.parser.element import Element
from bzd.parser.error import assertHasAttr
from tools.bdl.entity.type import Type
from tools.bdl.entity.contract import Contracts


class Using:

	def __init__(self, element: Element) -> None:

		assertHasAttr(element=element, attr="name")
		assertHasAttr(element=element, attr="type")

		self.element = element

	@property
	def category(self) -> str:
		return "using"

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def contracts(self) -> Contracts:
		return Contracts(sequence=self.element.getNestedSequence("contract"))

	@property
	def type(self) -> Type:
		return Type(element=self.element)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

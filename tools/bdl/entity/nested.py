import typing

from bzd.parser.element import Element
from bzd.parser.error import assertHasAttr
from tools.bdl.entity.type import Type


class Nested:

	def __init__(self, element: Element, nested: typing.List[typing.Any], visitor: typing.Any) -> None:

		assertHasAttr(element=element, attr="name")
		assertHasAttr(element=element, attr="type")

		self.element = element
		self.nested = nested
		self.visitor = visitor

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def type(self) -> Type:
		return Type(element=self.element)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.visitor.visitComment("nested", self.element.getAttrValue("comment"))

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entity.fragment.type import Type
from tools.bdl.entity.entity import Entity


class Nested(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)

		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="type")

		self.nested: typing.List[typing.Any] = []

	def setNested(self, nested: typing.List[typing.Any]) -> None:
		self.nested = nested

	@property
	def category(self) -> str:
		return "nested"

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

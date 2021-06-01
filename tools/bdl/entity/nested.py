import typing

from bzd.parser.element import Element
from bzd.parser.error import assertHasAttr
from tools.bdl.entity.type import Type


class Nested:

	def __init__(self, element: Element, nested: typing.List[typing.Any]) -> None:

		assertHasAttr(element=element, attr="name")
		assertHasAttr(element=element, attr="type")

		self.element = element
		self.nested = nested

	@property
	def symbol(self) -> str:
		return self.name

	@property
	def category(self) -> str:
		return "nested"

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.entity import Entity


class Nested(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)

		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="type")

		self.nested: typing.Any = []

	def setNested(self, nested: typing.Any) -> None:
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

	def __repr__(self) -> str:
		content = self.toString({"name": self.name, "type": self.type})
		content += "\n  ".join([""] + repr(self.nested).split("\n"))
		return content

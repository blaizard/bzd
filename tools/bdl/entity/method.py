import typing

from bzd.parser.element import Element
from bzd.parser.error import Error
from tools.bdl.entity.type import Type
from tools.bdl.entity.variable import Variable


class Method:

	def __init__(self, element: Element) -> None:

		Error.assertHasAttr(element=element, attr="name")
		self.element = element

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def category(self) -> str:
		return "method"

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@property
	def type(self) -> typing.Optional[Type]:
		return Type(element=self.element) if self.isType else None

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def args(self) -> typing.List[Variable]:
		if self.element.isNestedSequence("argument"):
			sequence = self.element.getNestedSequence("argument")
			assert sequence is not None
			return [Variable(element=arg) for arg in sequence.iterate()]
		return []

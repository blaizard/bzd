import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.variable import Variable
from tools.bdl.entities.impl.entity import Entity


class Method(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def category(self) -> str:
		return "method"

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@property
	def type(self) -> typing.Optional[Type]:
		return Type(element=self.element, kind="type", template="template") if self.isType else None

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

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

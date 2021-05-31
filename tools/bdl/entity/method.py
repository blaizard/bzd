import typing

from bzd.parser.element import Element
from bzd.parser.error import assertHasAttr
from tools.bdl.entity.type import Type
from tools.bdl.entity.variable import Variable


class Method:

	def __init__(self, element: Element, visitor: typing.Any) -> None:

		assertHasAttr(element=element, attr="name")

		self.element = element
		self.visitor = visitor

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@property
	def type(self) -> typing.Optional[Type]:
		return Type(element=self.element) if self.isType else None

	@property
	def comment(self) -> typing.Optional[str]:
		comment = self.visitor.visitComment(context=Method, comment=self.element.getAttrValue("comment"))
		return typing.cast(typing.Optional[str], comment)

	@property
	def args(self) -> typing.List[Variable]:
		if self.element.isNestedSequence("argument"):
			sequence = self.element.getNestedSequence("argument")
			assert sequence is not None
			return [Variable(element=arg, visitor=self.visitor) for arg in sequence.iterate()]
		return []

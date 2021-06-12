import typing

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.entity import Entity


class _VisitorInheritance(Visitor[Type, typing.List[Type]]):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[Type]:
		return []

	def visitElement(self, element: Element, result: typing.List[Type]) -> typing.List[Type]:
		Error.assertHasAttr(element=element, attr="symbol")
		result.append(Type(element=element, kind="symbol"))
		return result


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

	@property
	def hasInheritance(self) -> bool:
		return self.element.isNestedSequence("inheritance")

	@property
	def inheritanceList(self) -> typing.List[Type]:
		sequence = self.element.getNestedSequence("inheritance")
		if sequence is None:
			return []
		return _VisitorInheritance().visit(sequence=sequence)

	def __repr__(self) -> str:
		content = self.toString({
			"name": self.name,
			"type": self.type,
			"inheritance": ", ".join([str(t) for t in self.inheritanceList])
		})
		content += "\n  ".join([""] + repr(self.nested).split("\n"))
		return content

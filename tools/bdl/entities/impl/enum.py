import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor as VisitorBase

from tools.bdl.entities.impl.entity import Entity


class EnumValue(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element)
		Error.assertHasAttr(element=element, attr="name")

	@cached_property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")


class _Visitor(VisitorBase[str, typing.List[str]]):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		Error.assertHasAttr(element=element, attr="name")
		result.append(element.getAttr("name").value)
		return result


class Enum(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element)
		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasSequence(element=element, sequence="values")

	@property
	def category(self) -> str:
		return "enum"

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@cached_property
	def values(self) -> typing.Iterable[EnumValue]:
		sequence = self.element.getNestedSequence("values")
		assert sequence is not None
		return [EnumValue(element) for element in sequence]

	def __repr__(self) -> str:
		return self.toString({"name": self.name, "values": ", ".join([e.name for e in self.values])})

import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor as VisitorBase

from bdl.entities.impl.entity import Entity, Role


class _Visitor(VisitorBase[str, typing.List[str]]):
	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		Error.assertHasAttr(element=element, attr="name")
		result.append(element.getAttr("name").value)
		return result


class Namespace(Entity):
	"""
    A namespace defines a scope in which entities are available.
    - Sequence:
            - name: The list of names constituing the namespace.
    """

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasSequence(element=element, sequence="name")

	@property
	def nameList(self) -> typing.List[str]:
		sequence = self.element.getNestedSequence("name")
		assert sequence is not None
		return _Visitor().visit(sequence=sequence)

	def __repr__(self) -> str:
		return self.toString({"name": ".".join(self.nameList)})

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor as VisitorBase

from bdl.entities.impl.entity import Entity, Role


class _Visitor(VisitorBase[str, list[str]]):
	nestedKind = None

	def visitBegin(self, result: typing.Any) -> list[str]:
		return []

	def visitElement(self, element: Element, result: list[str]) -> list[str]:
		Error.assertHasAttr(element=element, attr="name")
		result.append(element.getAttr("name").value)
		return result


class Namespace(Entity):
	"""
	A namespace defines a scope in which entities are available.
	- Sequence:
	        - name: The list of names constituting the namespace.
	"""

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasSequence(element=element, sequence="name")

	@property
	def nameList(self) -> list[str]:
		sequence = self.element.getNestedSequence("name")
		assert sequence is not None
		return _Visitor().visit(sequence=sequence)

	def __repr__(self) -> str:
		return self.toString({"name": ".".join(self.nameList)})

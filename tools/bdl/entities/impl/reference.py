import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.entity import Entity, Role


class Reference(Entity):
	"""
	Reference element that points to another, this is only for internal use.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Meta)
		Error.assertHasAttr(element=element, attr="name")

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

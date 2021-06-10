from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entity.entity import Entity


class Builtin(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def category(self) -> str:
		return "builtin"

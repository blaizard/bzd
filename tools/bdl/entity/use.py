import typing
from pathlib import Path

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entity.entity import Entity


class Use(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)
		Error.assertHasAttr(element=element, attr="value")

	@property
	def category(self) -> str:
		return "use"

	@property
	def path(self) -> Path:
		return Path(self.element.getAttr("value").value)

import typing
from pathlib import Path

from bzd.utils.memoized_property import memoized_property
from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.entity import Entity


class Use(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)
		Error.assertHasAttr(element=element, attr="value")

	@property
	def category(self) -> str:
		return "use"

	@memoized_property
	def path(self) -> Path:
		return Path(self.element.getAttr("value").value)

	def __repr__(self) -> str:
		return self.toString({"path": self.path.as_posix()})

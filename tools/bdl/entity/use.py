import typing
from pathlib import Path

from bzd.parser.element import Element
from bzd.parser.error import assertHasAttr


class Use:

	def __init__(self, element: Element) -> None:

		assertHasAttr(element=element, attr="value")

		self.element = element

	@property
	def category(self) -> str:
		return "use"

	@property
	def path(self) -> Path:
		return Path(self.element.getAttr("value").value)

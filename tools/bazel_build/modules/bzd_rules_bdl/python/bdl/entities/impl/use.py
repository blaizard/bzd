import typing
from pathlib import Path
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error

from bdl.entities.impl.entity import Entity, Role


class Use(Entity):
	"""
    A use statement is equivalent to include or import in other languages.
    - Attributes:
            - value: The path the be included.
    """

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Meta)
		Error.assertHasAttr(element=element, attr="value")

	@property
	def path(self) -> Path:
		return Path(self.element.getAttr("value").value)

	def __repr__(self) -> str:
		return self.toString({"path": self.path.as_posix()})

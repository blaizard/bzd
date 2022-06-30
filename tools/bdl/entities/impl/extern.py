import typing
from pathlib import Path
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.types import TypeCategory


class Extern(Entity):
	"""
	A extern statement is used to pull a symbol coming from an external file (not interpretable by BDL).
	- Attributes:
		- type: The type of external symbol.
		- name: The name of the symbol.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="type")
		Error.assertHasAttr(element=element, attr="name")

	@property
	def category(self) -> str:
		return "extern"

	@property
	def typeCategory(self) -> TypeCategory:
		if self.type == "type":
			return TypeCategory.struct
		return TypeCategory(self.type)

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	def resolve(self, resolver: typing.Any) -> None:
		"""
		Resolve entities.
		"""
		self._setUnderlyingType(self.fqn)

		super().resolve(resolver)

	def __repr__(self) -> str:
		return self.toString({"name": self.name, "type": self.type})

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.types import TypeCategory
from tools.bdl.entities.impl.entity import Entity, Role


class Builtin(Entity):
	"""
	Builtin types, cannot be created by the user, but are available implicitly in the language.
	- Attributes:
		- name: The name of the buildin type.
	- Sequence:
		- [config]: Configuration for the type.
	"""

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def underlyingType(self) -> str:
		return self.name

	@property
	def category(self) -> str:
		return "builtin"

	@property
	def typeCategory(self) -> TypeCategory:
		return TypeCategory.builtin

	@property
	def isFQN(self) -> bool:
		return True

	@property
	def fqn(self) -> str:
		return self.name

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

from bdl.entities.impl.entity import Entity, Role
from bdl.entities.impl.fragment.symbol import Symbol


class Builtin(Entity):
	"""
    Builtin types, cannot be created by the user, but are available implicitly in the language.
    - Attributes:
            - name: The name of the built-in type.
            - meta: Whether this type is a meta type or not.
    - Sequence:
            - [config]: Configuration for the type.
    """

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def underlyingTypeFQN(self) -> str:
		return self.name

	@property
	def isSymbol(self) -> bool:
		return True

	@property
	def symbol(self) -> Symbol:
		return Symbol(element=self.element, kind="name", underlyingTypeFQN="name")

	@property
	def isFQN(self) -> bool:
		return True

	@property
	def fqn(self) -> str:
		return self.name

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

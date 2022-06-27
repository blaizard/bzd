import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.types import TypeCategory


class Using(Entity):
	"""
	Using statement are defining a type based on an underlying type.
	- Attributes:
		- name: The name of the new type, the one created by this entity.
		- type: The underlying type.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="type")

	@property
	def category(self) -> str:
		return "using"

	@property
	def typeCategory(self) -> TypeCategory:
		return TypeCategory.using

	@property
	def contracts(self) -> Contracts:
		return self.type.contracts

	@cached_property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", underlyingType="fqn_type", template="template", const="const")

	def resolve(self, resolver: typing.Any) -> None:
		"""
		Resolve entities.
		"""
		entity = self.type.resolve(resolver=resolver)

		super().resolve(resolver)

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

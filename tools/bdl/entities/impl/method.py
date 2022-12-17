import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.types import TypeCategory


class Method(Entity):
	"""
	A method is a definition of a functional object.
	- Attributes:
		- name: The name of the method.
		- [type]: The return type if any.
	- Sequence:
		- argument: The list of arguments to this method.
	"""

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def configAttr(self) -> str:
		return "argument"

	@property
	def typeCategory(self) -> TypeCategory:
		return TypeCategory.method

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@cached_property
	def type(self) -> typing.Optional[Type]:
		return Type(element=self.element,
			kind="type",
			underlyingTypeFQN="fqn_return_type",
			template="template",
			contract="contract_return",
			const="const") if self.isType else None

	def resolve(self, resolver: typing.Any) -> None:
		"""
		Resolve entities.
		"""
		# Generate this symbol FQN
		self._setUnderlyingTypeFQN(self.fqn)

		maybeType = self.type
		if maybeType is not None:
			entity = maybeType.resolve(resolver=resolver)

		self.parameters.resolve(resolver=resolver, EntityType=Expression)

		# Validate the type of arguments.
		parameterTypeCategories = {*self.parameters.getUnderlyingTypeCategories(resolver)}
		self.assertTrue(condition=TypeCategory.component not in parameterTypeCategories,
			message=f"Components are not allowed as method parameter.")

		super().resolve(resolver)

	@cached_property
	def parameters(self) -> Parameters:
		return Parameters(element=self.element, nestedKind="argument")

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

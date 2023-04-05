import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.types import Category


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
	def isSymbol(self) -> bool:
		return self.element.isAttr("symbol")

	@property
	def symbol(self) -> typing.Optional[Symbol]:
		return Symbol(element=self.element,
		              kind="symbol",
		              underlyingTypeFQN="fqn_return_type",
		              template="template",
		              contract="contract_return",
		              const="const") if self.isSymbol else None

	def resolve(self, resolver: typing.Any) -> None:
		"""
		Resolve entities.
		"""
		# Generate this symbol FQN
		self._setUnderlyingTypeFQN(self.fqn)

		maybeSymbol = self.symbol
		if maybeSymbol is not None:
			entity = maybeSymbol.resolve(resolver=resolver)

		self.parameters.resolve(resolver=resolver)

		# Validate the type of arguments.
		parameterTypeCategories = {*self.parameters.getUnderlyingTypeCategories(resolver)}
		self.assertTrue(condition=Category.component not in parameterTypeCategories,
		                message=f"Components are not allowed as method parameter.")

		super().resolve(resolver)

	@property
	def parameters(self) -> Parameters:
		return Parameters(element=self.element, NestedElementType=Expression, nestedKind="argument")

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

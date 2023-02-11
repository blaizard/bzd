import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.types import Category
from tools.bdl.entities.impl.fragment.contract import Contracts

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.symbol_map import Resolver
	from tools.bdl.entities.impl.expression import Expression

class ExpressionFragment(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Value)

	@staticmethod
	def make(element: Element) -> "ExpressionFragment":
		"""Make an ExpressionFragment from an element."""

		if element.isAttr("value"):
			return ValueFragment(element)
		elif element.isAttr("symbol"):
			return SymbolFragment(element)
		
		ExpressionFragment(element).error(message=f"Unsupported expression fragment for element: '{element}'.")

	def resolve(self, resolver: "Resolver") -> None:
		pass

	def toElement(self, element: "Element") -> None:
		"""Applies the fragment to an expression element."""

		# Copy attributes.
		elementBuilder = ElementBuilder.cast(element, ElementBuilder)
		for attr in ("symbol", "value", "symbol_category", "meta", "literal", "const", "fqn_type", "fqn_value"):
			if self.element.isAttr(attr):
				elementBuilder.setAttr(attr, self.element.getAttr(attr).value)

		# Copy sequences.
		for nested in ("argument", "argument_resolved", "argument_expected", "template", "template_resolved", "template_expected"):
			sequence = self.element.getNestedSequence(nested)
			if sequence:
				elementBuilder.setNestedSequence(nested, sequence)

		# Merge contracts with the existing ones if any, otherwise just create it.
		contracts = Contracts(element=element, sequenceKind="contract")
		contracts.resolve(self.contracts)

	def error(self, message: str) -> None:
		Error.handleFromElement(element=self.element, message=message)

	def assertTrue(self, condition: bool, message: str) -> None:
		Error.assertTrue(condition=condition,
			element=self.element,
			message=message)

class ValueFragment(ExpressionFragment):

	@property
	def value(self) -> str:
		return self.element.getAttr("value").value

	def resolve(self, resolver: "Resolver") -> None:
		self._setLiteral(self.value)

class SymbolFragment(ExpressionFragment):

	@property
	def const(self) -> bool:
		return self.symbol.const

	@cached_property
	def isParameters(self) -> bool:
		return self.element.isNestedSequence("argument")

	@cached_property
	def parameters(self) -> typing.Optional[Parameters]:
		"""Return the Parameters object if there are parameters. In case the expression
		is declared with empty parenthesis or without the Parameters object will be empty.
		"""
		from tools.bdl.entities.impl.expression import Expression
		if self.isParameters:
			return Parameters(element=self.element, NestedElementType=Expression, nestedKind="argument")
		return Parameters(element=self.element, NestedElementType=Expression)

	@property
	def symbol(self) -> str:
		return Symbol(element=self.element,
			kind="symbol",
			underlyingTypeFQN="fqn_type",
			template="template",
			const="const",
			contract="contract")

	def resolve(self, resolver: "Resolver") -> None:
		entity = self.symbol.resolve(resolver=resolver, maybeValue=True)
		if entity.isRoleMeta:
			self._setMeta()

		# The type refers to a value.
		if entity.isRoleValue:
			self.assertTrue(condition=self.isParameters == False,
				message="Cannot instantiate a value from another value.")
			# It means it refers directly to the entity, in that case it must have a value FQN
			if entity.underlyingValueFQN is None:
				self.assertTrue(condition=entity.isFQN, message="A value referenced must have an valid FQN.")
				self._setUnderlyingValueFQN(self.symbol.fqn)
			else:
				self._setUnderlyingValueFQN(entity.underlyingValueFQN)
			self._setLiteral(entity.literal)

		# The type refers to an actual type and will be instantiated as part of this expression.
		elif entity.isRoleType:

			# Generate the argument list and resolve it.
			assert self.parameters is not None
			self.parameters.resolve(resolver=resolver)
			self._resolveAndValidateParameters(resolver, entity, self.parameters)

		else:
			self.error(message=f"Cannot create an expression from '{entity.fqn}'.")

	def getConfigValues(self, resolver: "Resolver") -> Parameters:
		"""Get the list of expressions that forms the values."""

		from tools.bdl.entities.impl.expression import Expression
		if self.underlyingTypeFQN:
			underlyingTypeEntity = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(element=self.element)
			return Parameters(element=underlyingTypeEntity.element,
				NestedElementType=Expression,
				nestedKind=underlyingTypeEntity.configAttr,
				filterFct=lambda entity: entity.category == Category.expression)
		return Parameters(element=self.element, NestedElementType=Expression)


	def _resolveAndValidateParameters(self, resolver: "Resolver", resolvedTypeEntity: Entity,
		parameters: Parameters) -> None:
		"""Resolve and validate tthe parameters passed into argument."""

		# Make the resolved parameters before the validation is completed. This is because
		# it might make use of the parametersResolved.
		expectedParameters = self.getConfigValues(resolver=resolver)
		parameters.makeParametersResolved(name="argument", resolver=resolver, expected=expectedParameters)

		# Validate the type of arguments.
		parameterTypeCategories = {*parameters.getUnderlyingTypeCategories(resolver)}
		if Category.component in parameterTypeCategories:
			category = resolvedTypeEntity.category
			self.assertTrue(condition=category in [Category.component, Category.method, Category.builtin],
				message=f"Components are not allowed for this type '{category}'.")

		# Read the validation for the values and evaluate it.
		validation = expectedParameters.makeValidationForValues(resolver=resolver)
		arguments = parameters.getValuesOrTypesAsDict(resolver=resolver, varArgs=False)
		result = validation.validate(arguments, output="return")
		Error.assertTrue(element=self.element, attr="symbol", condition=bool(result), message=str(result))

		# Compute and set the literal value if any.
		maybeValue = resolvedTypeEntity.toLiteral(result.values)  # type: ignore
		if maybeValue is not None:
			self.assertTrue(condition=isinstance(maybeValue, str),
				message=f"The returned value from toLiteral must be a string, not {str(maybeValue)}")
			self._setLiteral(maybeValue)

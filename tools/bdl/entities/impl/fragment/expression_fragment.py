import typing
import re
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.entity import Entity, EntityExpression, Role
from tools.bdl.entities.impl.types import Category
from tools.bdl.entities.impl.fragment.contract import Contracts

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.symbol_map import Resolver
	from tools.bdl.entities.impl.expression import Expression


class ExpressionFragment(EntityExpression):

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Value)

	@staticmethod
	def make(element: Element) -> "ExpressionFragment":  # type: ignore
		"""Make an ExpressionFragment from an element."""

		if element.isAttr("value"):
			return ValueFragment(element)
		elif element.isAttr("symbol"):
			return SymbolFragment(element)
		elif element.isAttr("operator"):
			return OperatorFragment(element)
		elif element.isAttr("regexpr"):
			return RegexprFragment(element)

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
		for nested in ("argument", "argument_resolved", "argument_expected", "template", "template_resolved",
		               "template_expected"):
			sequence = self.element.getNestedSequence(nested)
			if sequence:
				elementBuilder.setNestedSequence(nested, sequence)

		# Merge contracts with the existing ones if any, otherwise just create it.
		contracts = Contracts(element=element, sequenceKind="contract")
		contracts.resolve(self.contracts)

	def unary(self, operator: "OperatorFragment") -> None:
		"""Support for unary operators."""

		literal = self.literalNative
		self.assertTrue(condition=isinstance(literal, (int, float)),
		                message="Unary operators can only be used with numbers.")
		if operator.operator == '+':
			pass
		elif operator.operator == '-':
			self._setLiteral(str(-literal))  # type: ignore
		else:
			self.error(f"Unsupported unary operator '{operator.operator}'.")

	def binary(self, operator: "OperatorFragment", fragment: "ExpressionFragment") -> None:
		"""Support for binary operators."""

		literal1 = self.literalNative
		literal2 = fragment.literalNative
		self.assertTrue(condition=isinstance(literal1, (int, float)),
		                message="Bainry operators can only be used with numbers.")
		fragment.assertTrue(condition=isinstance(literal2, (int, float)),
		                    message="Bainry operators can only be used with numbers.")
		if operator.operator == '+':
			self._setLiteral(str(literal1 + literal2))  # type: ignore
		elif operator.operator == '-':
			self._setLiteral(str(literal1 - literal2))  # type: ignore
		elif operator.operator == '*':
			self._setLiteral(str(literal1 * literal2))  # type: ignore
		elif operator.operator == '/':
			self._setLiteral(str(literal1 / literal2))  # type: ignore
		else:
			self.error(f"Unsupported binary operator '{operator.operator}'.")


class OperatorFragment(ExpressionFragment):
	pass


class ValueFragment(ExpressionFragment):

	def resolve(self, resolver: "Resolver") -> None:
		self._setLiteral(self.value)


class RegexprFragment(ExpressionFragment):

	@property
	def regexpr(self) -> str:
		return self.element.getAttr("regexpr").value

	def resolve(self, resolver: "Resolver") -> None:
		try:
			re.compile(self.regexpr)
		except re.error:
			self.error("Invalid regular expression.")


class SymbolFragment(ExpressionFragment):

	@cached_property
	def parameters(self) -> typing.Optional[Parameters]:
		"""
		Return the Parameters object if there are parameters. In case the expression
		is declared with empty parenthesis or without the Parameters object will be empty.
		"""
		from tools.bdl.entities.impl.expression import Expression
		if self.isParameters:
			return Parameters(element=self.element, NestedElementType=Expression, nestedKind="argument")
		return Parameters(element=self.element, NestedElementType=Expression)

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
			underlyingTypeEntity = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(
			    element=self.element)
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

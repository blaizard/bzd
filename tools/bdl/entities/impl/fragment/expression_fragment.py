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
		elif element.isAttr("preset"):
			return PresetFragment(element)

		ExpressionFragment(element).error(message=f"Unsupported expression fragment for element: '{element}'.")

	def resolve(self, resolver: "Resolver") -> None:
		pass

	def toElement(self, element: "Element") -> None:
		"""Applies the fragment to an expression element."""

		# Copy attributes.
		elementBuilder = ElementBuilder.cast(element, ElementBuilder)
		for attr in ("interface", "symbol", "value", "symbol_category", "symbol_resolved", "meta", "literal", "const",
		             "fqn_type", "fqn_value"):
			if self.element.isAttr(attr):
				elementBuilder.setAttr(attr, self.element.getAttr(attr).value)

		# Copy sequences.
		for nested in ("argument", "argument_resolved", "argument_expected", "template", "template_resolved",
		               "template_expected", "regexpr_include", "regexpr_exclude"):
			sequence = self.element.getNestedSequence(nested)
			if sequence:
				elementBuilder.setNestedSequence(nested, sequence)

		# Merge contracts with the existing ones if any, otherwise just create it.
		contracts = Contracts(element=element, sequenceKind="contract")
		contracts.merge(self.contracts)

	def unary(self, operator: "OperatorFragment") -> None:
		"""Support for unary operators."""

		literal = self.literalNative
		if isinstance(literal, (int, float)):
			if operator.operator == '+':
				pass
			elif operator.operator == '-':
				self._setLiteral(str(-literal))
			else:
				self.error(f"Unsupported unary operator with number '{operator.operator}'.")
			return

		regexpr = self.regexpr
		if regexpr.isValid:
			if operator.operator == "+":
				pass
			elif operator.operator == "-":
				regexpr.negate()
			else:
				self.error(f"Unsupported unary operator with regexpr '{operator.operator}'.")

		self.error(f"Unsupported unary operator '{operator.operator}'.")

	def binary(self, operator: "OperatorFragment", fragment: "ExpressionFragment") -> None:
		"""Support for binary operators."""

		literal1 = self.literalNative
		literal2 = fragment.literalNative
		if isinstance(literal1, (int, float)) and isinstance(literal2, (int, float)):
			if operator.operator == '+':
				self._setLiteral(str(literal1 + literal2))
			elif operator.operator == '-':
				self._setLiteral(str(literal1 - literal2))
			elif operator.operator == '*':
				self._setLiteral(str(literal1 * literal2))
			elif operator.operator == '/':
				self._setLiteral(str(literal1 / literal2))
			else:
				self.error(f"Unsupported binary operator with numbers '{operator.operator}'.")
			return

		regexpr1 = self.regexpr
		regexpr2 = fragment.regexpr
		if regexpr1.isValid and regexpr2.isValid:
			self.assertTrue(condition=operator.operator in ("+", "-"),
			                message=f"Unsupported binary operator with regexprs '{operator.operator}'.")
			if operator.operator == "-":
				regexpr2.negate()
			regexpr1.merge(regexpr2)
			return

		self.error(f"Unsupported binary operator '{operator.operator}'.")


class OperatorFragment(ExpressionFragment):
	pass


class ValueFragment(ExpressionFragment):

	def resolve(self, resolver: "Resolver") -> None:
		self._setLiteral(self.value)


class RegexprFragment(ExpressionFragment):

	def resolve(self, resolver: "Resolver") -> None:

		try:
			re.compile(self.regexprAttr)
		except re.error:
			self.error("Invalid regular expression.")

		self.regexpr.addInclude(self.regexprAttr)


class SymbolFragment(ExpressionFragment):

	@property
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
				self._setUnderlyingValueFQN(str(self.symbol))
			else:
				self._setUnderlyingValueFQN(entity.underlyingValueFQN)

			# Propagate the literal value.
			self._setLiteral(entity.literal)

			# Propagate the regexpr.
			self._setRegexpr(entity.regexpr)

		# The type refers to an actual type and will be instantiated as part of this expression.
		elif entity.isRoleType:

			# Generate the argument list and resolve it.
			assert self.parameters is not None
			self.parameters.resolve(resolver=resolver)
			self._resolveAndValidateParameters(resolver, entity, self.parameters)

		else:
			self.error(message=f"Cannot create an expression from '{entity.fqn}'.")

	def getConfigValues(self, resolver: "Resolver") -> Parameters:
		"""Get the list of expressions that forms the values and resolve them."""

		from tools.bdl.entities.impl.expression import Expression
		if self.underlyingTypeFQN:
			underlyingTypeEntity = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(
			    element=self.element)
			params = Parameters(element=underlyingTypeEntity.element, NestedElementType=Expression)

			# Take into account the parents as well.
			for fqn in reversed([self.underlyingTypeFQN, *underlyingTypeEntity.getParents()]):
				parentEntity = resolver.getEntityResolved(fqn=fqn).assertValue(element=self.element)
				parentParams = Parameters(element=parentEntity.element,
				                          NestedElementType=Expression,
				                          nestedKind=parentEntity.configAttr,
				                          filterFct=lambda entity: entity.category == Category.expression)
				# Resolve the types from a config sequence if any.
				# These are resolved only when used, allowing symbol to be only available during composition.
				for entity in parentParams:
					typing.cast("Expression", entity).resolve(resolver=resolver.make(namespace=parentEntity.namespace))
				params.extend(parentParams)

			return params

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
			self.assertTrue(
			    condition=category in [Category.component, Category.method, Category.builtin],
			    message=
			    f"{resolvedTypeEntity}: Components as parameters are not allowed for entities of category '{category}'."
			)

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


class PresetFragment(SymbolFragment):

	def resolve(self, resolver: "Resolver") -> None:

		presets = {
		    "out": {
		        "interface": "bzd.OStream",
		        "symbol": "target.out"
		    },
		    "in": {
		        "interface": "bzd.IStream",
		        "symbol": "target.in"
		    },
		    "timer": {
		        "interface": "bzd.Timer",
		        "symbol": "target.timer"
		    },
		    "clock": {
		        "interface": "bzd.Clock",
		        "symbol": "target.clock"
		    },
		    "network.tcp.client": {
		        "interface": "bzd.network.tcp.Client",
		        "symbol": "target.network.tcp.client"
		    }
		}

		if self.preset in presets:
			for attr, value in presets[self.preset].items():
				ElementBuilder.cast(self.element, ElementBuilder).setAttr(attr, value)

		else:
			self.error(message=f"The preset '{self.preset}' is not supported.")

		super().resolve(resolver=resolver)

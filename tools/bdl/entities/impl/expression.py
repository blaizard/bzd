import typing
import enum
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

from tools.bdl.contracts.validation import Validation, SchemaDict
from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.entity import Entity, EntityExpression, Role
from tools.bdl.entities.impl.types import Category

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.symbol_map import Resolver


class Expression(EntityExpression):
	"""
	An expression can be:
		- a variable declaration.
		- a variable definition.
		- a method call.
		- a value.
	- Attributes:
		- [type]: The method name or variable type to be used.
		- [value]: The value this expression represents.
		- [name]: The resulting symbol name.
		- [const]: If the expression is constant.
		- [underlyingValueFQN]: The actual value discovered after resolution.
		- [symbol]: The interface type if any (might not be set).
		- [executor]: The executor for which this expression should run.
	- Sequence:
		- argument: The list of arguments to pass to the instanciation or method call.
		- argument_resolved: List of resolved arguments.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		self.assertTrue(condition=(self.isValue or self.isSymbol),
			message="Expression must represent a symbol or a value.")

	@cached_property
	def typeResolved(self) -> Symbol:
		return Symbol(element=self.element,
			kind="type",
			underlyingTypeFQN="fqn_type",
			template="template_resolved",
			const="const")

	@property
	def isInterfaceType(self) -> bool:
		return self.element.isAttr("symbol") and bool(self.element.getAttr("symbol").value)

	@cached_property
	def interfaceType(self) -> Symbol:
		return Symbol(element=self.element, kind="symbol", underlyingTypeFQN="fqn_interface",
			const="const") if self.isInterfaceType else self.symbol

	@cached_property
	def interfaceTypeResolved(self) -> Symbol:
		return Symbol(element=self.element, kind="symbol", underlyingTypeFQN="fqn_interface",
			const="const") if self.isInterfaceType else self.typeResolved

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this entity.
		"""
		dependencies = set()
		if self.isSymbol:
			dependencies.update(self.symbol.dependencies)
		if self.isParameters:
			assert self.parameters is not None
			dependencies.update(self.parameters.dependencies)

		return dependencies

	def resolve(self, resolver: "Resolver") -> None:
		"""
		Resolve entities.
		"""

		# Resolve the interface associated with this expression.
		if self.isInterfaceType:
			self.interfaceType.resolve(resolver=resolver)
			# TODO: Ensure that the interface is part of the parent type.

		# If it holds a value, it is considered a literal.
		if self.isValue:
			self._setLiteral(value=self.value)

		# If it holds a symbol.
		elif self.isSymbol:

			# Check if this points to a type or a value.
			entity = self.symbol.resolve(resolver=resolver, maybeValue=True)
			if entity.isRoleMeta:
				self._setMeta()

			# Set the executor.
			executorContract = self.contracts.get("executor")
			if executorContract is not None:
				self.assertTrue(condition=entity.category in {Category.component, Category.method},
					message=f"`executor` contracts must be set at component instantiation, not '{entity.category}'.")
				executor = executorContract.value
			# If there is a 'this', propagate the executor.
			elif self.symbol.isThis:
				executor = self.symbol.getThisResolved(resolver=resolver).executor
			else:
				executor = None
			if executor is not None:
				self._setExecutor(executor)

			# The type refers to a value.
			if entity.isRoleValue:
				self.assertTrue(condition=self.isParameters == False,
					message="Cannot instantiate a value from another value.")
				# It means it refers directly to the entity, in that case it must have a value FQN
				if entity.underlyingValueFQN is None:
					self.assertTrue(condition=entity.isFQN, message="A value referenced must have an valid FQN.")
					self._setUnderlyingValueFQN(fqn=self.symbol.fqn)
				else:
					self._setUnderlyingValueFQN(fqn=entity.underlyingValueFQN)
				self._setLiteral(value=entity.literal)

			# The type refers to an actual type and will be instantiated as part of this expression.
			elif entity.isRoleType:

				# Generate the argument list and resolve it.
				assert self.parameters is not None
				self.parameters.resolve(resolver=resolver)
				self._resolveAndValidateParameters(resolver, entity, self.parameters)

			else:
				self.error(message=f"Cannot create an expression from '{entity.fqn}'.")

			super().resolve(resolver)

		else:
			self.error(message="Unsupported expression.")

		# Validate the whole expression with the contracts if any.
		if self.contracts.validationForValue:
			validation = Validation(schema=[self.contracts.validationForValue], args={"resolver": resolver})
			result = validation.validate([self], output="return")
			self.assertTrue(condition=bool(result), message=str(result))

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
		validation = self.makeValidationForValues(resolver=resolver, parameters=expectedParameters)
		arguments = parameters.getValuesOrTypesAsDict(resolver=resolver, varArgs=False)
		result = validation.validate(arguments, output="return")
		Error.assertTrue(element=self.element, attr="type", condition=bool(result), message=str(result))

		# Compute and set the literal value if any.
		maybeValue = resolvedTypeEntity.toLiteral(result.values)  # type: ignore
		if maybeValue is not None:
			self.assertTrue(condition=isinstance(maybeValue, str),
				message=f"The returned value from toLiteral must be a string, not {str(maybeValue)}")
			self._setLiteral(maybeValue)

	@cached_property
	def parameters(self) -> typing.Optional[Parameters]:
		"""
		Return the Parameters object if there are parameters. In case the expression
		is declared with empty parenthesis or without the Parameters object will be empty.
		"""
		if self.isParameters:
			return Parameters(element=self.element, NestedElementType=Expression, nestedKind="argument")
		return Parameters(element=self.element, NestedElementType=Expression)

	@cached_property
	def parametersResolved(self) -> ParametersResolved:
		return ParametersResolved(element=self.element,
			NestedElementType=Expression,
			param="argument_resolved",
			expected="argument_expected")

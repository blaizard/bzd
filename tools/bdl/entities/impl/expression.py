import typing
import enum
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

from tools.bdl.contracts.validation import Validation, SchemaDict
from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters, ResolvedParameters
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.entity import Entity, EntityExpression, Role
from tools.bdl.entities.impl.types import TypeCategory

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
	- Sequence:
		- argument: The list of arguments to pass to the instanciation or method call.
		- argument_resolved: List of resolved arguments.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		self.assertTrue(condition=(self.isValue or self.isType), message="Expression must represent a type or a value.")

	@property
	def executor(self) -> str:
		maybeContract = self.contracts.get("executor")
		if maybeContract is not None:
			maybeExecutor = maybeContract.value
			if maybeExecutor is not None:
				return maybeExecutor
		return "executor"

	@cached_property
	def typeResolved(self) -> Type:
		return Type(element=self.element,
			kind="type",
			underlyingTypeFQN="fqn_type",
			template="template_resolved",
			const="const")

	@property
	def isInterfaceType(self) -> bool:
		return self.element.isAttr("symbol") and bool(self.element.getAttr("symbol").value)

	@cached_property
	def interfaceType(self) -> Type:
		return Type(element=self.element, kind="symbol", underlyingTypeFQN="fqn_interface",
			const="const") if self.isInterfaceType else self.type

	@cached_property
	def interfaceTypeResolved(self) -> Type:
		return Type(element=self.element, kind="symbol", underlyingTypeFQN="fqn_interface",
			const="const") if self.isInterfaceType else self.typeResolved

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this entity.
		"""
		dependencies = set()
		if self.isType:
			dependencies.update(self.type.dependencies)
		if self.isParameters:
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
			if self.isFQN:
				self._setUnderlyingValueFQN(fqn=self.fqn)

		# If it holds a type.
		elif self.isType:

			# Check if this points to a type or a value.
			entity = self.type.resolve(resolver=resolver, maybeValue=True)
	
			# The type refers to a value.
			if entity.isRoleValue:
				self.assertTrue(condition=self.isParameters == False, message="Cannot instantiate a value from another value.")
				self.assertTrue(condition=entity.underlyingValueFQN, message="A value referenced must have an underlying value FQN.")
				self._setUnderlyingValueFQN(fqn=entity.underlyingValueFQN)
				self._setLiteral(value=entity.literal)

			# The type refers to an actual type and will be instantiated as part of this expression.
			elif entity.isRoleType:

				# If this is a temporary (hence no FQN), no need to set the underlying FQN, this value will never be referenced.
				# The underlying value FQN is only needed for expressions that can be referenced.
				if self.isFQN:
					self._setUnderlyingValueFQN(fqn=self.fqn)

				# Generate the argument list and resolve it.
				self.parameters.resolve(resolver=resolver)
				self._resolveAndValidateParameters(resolver, entity, self.parameters)

			else:
				self.error(message=f"Cannot create an expression from '{entity.fqn}'.")

			super().resolve(resolver)

		else:
			self.error(message="Unsupported expression.")

	def _resolveAndValidateParameters(self, resolver: "Resolver", resolvedTypeEntity: Entity,
		parameters: Parameters) -> None:
		"""Resolve and validate tthe parameters passed into argument."""

		# Merge its default values
		argumentConfig = self.getConfigValues(resolver=resolver)
		parameters.mergeDefaults(defaults=argumentConfig)

		# Validate the type of arguments.
		parameterTypeCategories = {*parameters.getUnderlyingTypeCategories(resolver)}
		if TypeCategory.component in parameterTypeCategories:
			typeCategory = resolvedTypeEntity.typeCategory  # type: ignore
			self.assertTrue(condition=typeCategory
				in [TypeCategory.component, TypeCategory.method, TypeCategory.builtin],
				message=f"Components are not allowed for this type '{typeCategory}'.")

		# Read the validation for the value. it comes in part from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		validation = self._makeValueValidation(resolver=resolver, parameters=argumentConfig, contracts=self.contracts)
		if validation is not None:
			arguments = parameters.getValuesOrTypesAsDict(resolver=resolver, varArgs=False)
			result = validation.validate(arguments, output="return")
			Error.assertTrue(element=self.element, attr="type", condition=bool(result), message=str(result))
			maybeValue = resolvedTypeEntity.toLiteral(result.values)
			if maybeValue is not None:
				self._setLiteral(maybeValue)

		# Save the resolved parameters (values and templates), only after the validation is completed.
		argumentValues = parameters.copy()
		sequenceValues = argumentValues.toResolvedSequence(resolver=resolver, varArgs=False)
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("argument_resolved", sequenceValues)

		configValues = argumentConfig.copy()
		sequence = configValues.toResolvedSequence(resolver=resolver, varArgs=True)
		sequence += [sequence[-1]] * (len(sequenceValues) - len(sequence)) if configValues.isVarArgs else []
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("argument_expected", sequence)

	def _makeValueValidation(self, resolver: "Resolver", parameters: Parameters,
		contracts: Contracts) -> typing.Optional[Validation[SchemaDict]]:
		"""
		Generate the validation for the value by combining the type validation
		and the contract validation.
		"""

		# The validation comes from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		validationValue = contracts.validationForValue

		# Get the configuration value if any.
		if self.underlyingTypeFQN is not None:
			underlyingTypeFQN = resolver.getEntityResolved(self.underlyingTypeFQN).assertValue(element=self.element)
			if underlyingTypeFQN.isConfig:
				self.assertTrue(condition=not validationValue,
					message=f"This expression has both a global contract '{validationValue}' and a configuration, this is not allowed.")
				return self.makeValidationForValues(resolver=resolver, parameters=parameters)

		# If evaluates to true, meaning there is a contract for values,
		# it means there must be a single value.
		if validationValue:
			try:
				return Validation(schema={"0": validationValue}, args={"resolver": resolver})
			except Exception as e:
				self.error(message=str(e))

		# Validation is empty
		return Validation(schema={}, args={"resolver": resolver})

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
	def parametersResolved(self) -> ResolvedParameters:
		return ResolvedParameters(element=self.element, nestedKind="argument_resolved")

	@cached_property
	def parametersExpectedResolved(self) -> ResolvedParameters:
		return ResolvedParameters(element=self.element, nestedKind="argument_expected", allowMultiVarArgs=True)

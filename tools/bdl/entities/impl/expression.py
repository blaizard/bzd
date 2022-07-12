import typing
import enum
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from tools.bdl.contracts.validation import Validation, SchemaDict
from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters, ResolvedParameters
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.types import TypeCategory

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.symbol_map import Resolver


class Expression(Entity):
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
		- [underlyingValue]: The actual value discovered after resolution.
		- [symbol]: The interface type if any (might not be set).
	- Sequence:
		- argument: The list of arguments to pass to the instanciation or method call.
		- argument_resolved: List of resolved arguments.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		self.assertTrue(condition=(self.isValue or self.isType), message="Expression must represent a type or a value.")

	@property
	def category(self) -> str:
		return "expression"

	@property
	def executor(self) -> str:
		maybeContract = self.contracts.get("executor")
		if maybeContract is not None:
			maybeExecutor = maybeContract.value
			if maybeExecutor is not None:
				return maybeExecutor
		return "executor"

	@property
	def const(self) -> bool:
		return self.type.const

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name") and not self.name == "..."

	@property
	def isVarArgs(self) -> bool:
		return self.element.isAttr("name") and self.name == "..."

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@cached_property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", underlyingType="fqn_type", template="template", const="const")

	@cached_property
	def typeResolved(self) -> Type:
		return Type(element=self.element,
			kind="type",
			underlyingType="fqn_type",
			template="template_resolved",
			argumentTemplate="argument_template_resolved",
			const="const")

	@property
	def isInterfaceType(self) -> bool:
		return self.element.isAttr("symbol") and bool(self.element.getAttr("symbol").value)

	@cached_property
	def interfaceType(self) -> Type:
		return Type(element=self.element, kind="symbol", underlyingType="fqn_interface",
			const="const") if self.isInterfaceType else self.type

	@cached_property
	def interfaceTypeResolved(self) -> Type:
		return Type(element=self.element, kind="symbol", underlyingType="fqn_interface",
			const="const") if self.isInterfaceType else self.typeResolved

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def value(self) -> str:
		return self.element.getAttr("value").value

	@property
	def raw(self) -> str:
		return self.value if self.isValue else self.type.kind

	@property
	def literal(self) -> typing.Optional[str]:
		"""
		A literal type is type that can be described as a string.
		"""
		if self.isValue:
			return self.value
		return super().literal

	@property
	def isRoleValue(self) -> bool:
		return self.literal is not None or self.underlyingValue is not None

	@property
	def isRoleType(self) -> bool:
		return not bool(self.isRoleValue)

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this entity.
		"""
		dependencies = set()
		if self.isType:
			dependencies.update(self.type.dependencies)
		if self.isParameters:
			assert self.parameters is not None
			dependencies.update(self.parameters.dependencies)

		return dependencies

	def resolve(self, resolver: "Resolver") -> None:
		"""
		Resolve entities.
		"""
		if self.isValue:
			return

		entity = self.type.resolve(resolver=resolver)

		if self.isInterfaceType:
			self.interfaceType.resolve(resolver=resolver)

		# Set the underlying value
		if self.isParameters:

			# The type must represent a type (not a value) and have a valid FQN.
			self.assertTrue(condition=entity.isRoleType, message="Cannot instantiate a value from another value.")
			self.assertTrue(condition=self.isFQN, message="The value must have a FQN.")
			# TODO: it is possible that the expression does not have a FQN, in case the expression is resolved through another entity.
			self._setUnderlyingValue(entity=self, fqn=self.fqn)

		else:
			self._setUnderlyingValue(entity=entity)

		# Get the resolved type and check the kind of role it has.
		resolvedTypeEntity = resolver.getEntityResolved(fqn=self.typeResolved.fqn).assertValue(element=self.element)

		# For instanciation only, we want to resolve and validate the arguments.
		if resolvedTypeEntity.isRoleType:

			# Generate the argument list and resolve it.
			if self.isParameters:
				parameters = self.parameters
				assert parameters is not None
				parameters.resolve(resolver=resolver)
			else:
				parameters = Parameters(element=self.element)

			self._resolveAndValidateParameters(resolver, resolvedTypeEntity, parameters)

		# This is a type assignment, nothing to do here.
		elif resolvedTypeEntity.isRoleValue:
			pass

		else:
			self.error(message=f"Cannot create an expression from this element: {resolvedTypeEntity.fqn}")

		super().resolve(resolver)

	def _resolveAndValidateParameters(self, resolver: "Resolver", resolvedTypeEntity: Entity,
		parameters: Parameters) -> None:
		"""Resolve and validate tthe parameters passed into argument."""

		# Merge its default values
		argumentConfig = self.getConfigValues(resolver=resolver)
		parameters.mergeDefaults(argumentConfig)

		# Validate the type of arguments.
		parameterTypeCategories = {*parameters.getUnderlyingTypeCategories(resolver)}
		if TypeCategory.component in parameterTypeCategories:
			typeCategory = resolvedTypeEntity.typeCategory  # type: ignore
			self.assertTrue(condition=typeCategory in [TypeCategory.component],
				message=f"Components are not allowed for this type '{typeCategory}'.")

		# Read the validation for the value. it comes in part from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		validation = self._makeValueValidation(resolver=resolver, parameters=argumentConfig, contracts=self.contracts)
		if validation is not None:
			arguments = parameters.getValuesOrTypesAsDict(resolver=resolver, varArgs=False)
			result = validation.validate(arguments, output="return")
			Error.assertTrue(element=self.element, attr="type", condition=bool(result), message=str(result))

		# Save the resolved parameters (values and templates), only after the validation is completed.
		argumentValues = parameters.copy(template=False)
		sequenceValues = argumentValues.toResolvedSequence(resolver=resolver, varArgs=False, onlyValues=True)
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("argument_resolved", sequenceValues)

		argumentTemplates = parameters.copy(template=True)
		sequenceTemplates = argumentTemplates.toResolvedSequence(resolver=resolver, varArgs=False, onlyValues=True)
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("argument_template_resolved",
			sequenceTemplates)

		configValues = argumentConfig.copy(template=False)
		sequence = configValues.toResolvedSequence(resolver=resolver, varArgs=True, onlyValues=False)
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
		if self.underlyingType is not None:
			underlyingType = resolver.getEntityResolved(self.underlyingType).assertValue(element=self.element)
			if underlyingType.isConfig:
				self.assertTrue(condition=not validationValue,
					message="This expression has both a global contract and a configuration, this is not allowed.")
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
	def isParameters(self) -> bool:
		return self.element.isNestedSequence("argument")

	@cached_property
	def parameters(self) -> typing.Optional[Parameters]:
		"""
		Return the Parameters object if there are parameters. In case the expression
		is declared with empty parenthesis the Parameters object will be empty.
		In case the expression is defined without parenthesis, it returns None.
		"""
		if self.isParameters:
			return Parameters(element=self.element, nestedKind="argument")
		return None

	@cached_property
	def parametersResolved(self) -> ResolvedParameters:
		return ResolvedParameters(element=self.element, nestedKind="argument_resolved")

	@cached_property
	def parametersExpectedResolved(self) -> ResolvedParameters:
		return ResolvedParameters(element=self.element, nestedKind="argument_expected")

	def __repr__(self) -> str:
		return self.toString({
			"name": self.name if self.isName else "",
			"type": str(self.type) if self.isType else None,
			"value": str(self.value) if self.isValue else None
		})

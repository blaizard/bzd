import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters, ResolvedParameters
from tools.bdl.entities.impl.entity import Entity, Role

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.symbol_map import SymbolMap


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
	def const(self) -> bool:
		return self.element.isAttr("const")

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
		return Type(element=self.element, kind="type", underlyingType="fqn_type", template="template")

	@cached_property
	def typeResolved(self) -> Type:
		return Type(element=self.element,
			kind="type",
			underlyingType="fqn_type",
			template="template_resolved",
			argumentTemplate="argument_template_resolved")

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
		for params in self.parameters:
			dependencies.update(params.dependencies)

		return dependencies

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve entities.
		"""
		if self.isValue:
			return

		entity = self.type.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

		# Set the underlying value
		if bool(self.parameters):

			# The type must represent a type (not a value)
			self.assertTrue(condition=entity.isRoleType, message="Cannot instantiate a value from another value.")

			# Generate this symbol FQN
			# TODO: need to handle when the expression has no name
			fqn = symbols.namespaceToFQN(name=self.name, namespace=namespace)
			self._setUnderlyingValue(entity=self, fqn=fqn)

		else:
			self._setUnderlyingValue(entity=entity)

		# Generate the argument list
		self.parameters.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		defaults = self.getConfigValues(symbols=symbols)
		self.parameters.mergeDefaults(defaults)

		# Read the validation for the value. it comes in part from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		validation = self._makeValueValidation(symbols=symbols, parameters=defaults, contracts=self.contracts)
		if validation is not None:
			arguments = self.parameters.getValuesOrTypesAsDict(symbols=symbols, exclude=exclude)
			result = validation.validate(arguments, output="return")
			Error.assertTrue(element=self.element, attr="type", condition=bool(result), message=str(result))

		# Save the resolved parameters (values and templates), only after the validation is completed.
		argumentValues = self.parameters.copy(template=False)
		sequence = argumentValues.toResolvedSequence(symbols=symbols, exclude=exclude, onlyValues=True)
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("argument_resolved", sequence)

		argumentTemplates = self.parameters.copy(template=True)
		sequence = argumentTemplates.toResolvedSequence(symbols=symbols, exclude=exclude, onlyValues=True)
		ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("argument_template_resolved", sequence)

	def _makeValueValidation(self, symbols: typing.Any, parameters: Parameters,
		contracts: Contracts) -> typing.Optional[Validation]:
		"""
		Generate the validation for the value by combining the type validation
		and the contract validation.
		"""

		# The validation comes from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		validationValue = contracts.validationForValue

		# Get the configuration value if any.
		if self.underlyingType is not None:
			underlyingType = symbols.getEntityResolved(self.underlyingType).assertValue(element=self.element)
			if underlyingType.isConfig:
				self.assertTrue(condition=not validationValue,
					message="Value-specific contracts cannot be associated with a configuration.")
				return self.makeValidationForValues(symbols=symbols, parameters=parameters)

		# If evaluates to true, meaning there is a contract for values,
		# it means there must be a single value.
		if validationValue:
			try:
				return Validation(schema={"0": validationValue}, args={"symbols": symbols})
			except Exception as e:
				self.error(message=str(e))

		# Validation is empty
		return Validation(schema={}, args={"symbols": symbols})

	@cached_property
	def parameters(self) -> Parameters:
		return Parameters(element=self.element, nestedKind="argument")

	@cached_property
	def parametersResolved(self) -> ResolvedParameters:
		return ResolvedParameters(element=self.element, nestedKind="argument_resolved")

	def __repr__(self) -> str:
		return self.toString({
			"name": self.name if self.isName else "",
			"type": str(self.type) if self.isType else None,
			"value": str(self.value) if self.isValue else None
		})

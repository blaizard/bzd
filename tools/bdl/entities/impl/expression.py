import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.entity import Entity, Role

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.preprocess.symbol_map import SymbolMap


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
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@cached_property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", template="template")

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
		if len(self.args) == 1 and self.args[0].isValue:
			return self.args[0].value
		return super().literal

	@property
	def isRoleValue(self) -> bool:
		return self.literal is not None or self.underlyingValue is not None

	@property
	def isRoleType(self) -> bool:
		return not bool(self.isRoleValue)

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

		# Set the underlying type
		if entity.underlyingType is not None:
			self._setUnderlyingType(fqn=entity.underlyingType)

		# Set the underlying value
		if self.isArg:

			# The type must represent a type (not a value)
			self.assertTrue(condition=entity.isRoleType, message="Cannot instantiate a value from another value.")

			# Generate this symbol FQN
			# TODO: need to handle when the expression has no name
			fqn = symbols.makeFQN(name=self.name, namespace=namespace)
			self._setUnderlyingValue(entity=self, fqn=fqn)

		else:
			self._setUnderlyingValue(entity=entity)

		# Resolve contract
		self.contracts.mergeBase(entity.contracts)

		# Generate the argument list
		self.args.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		defaults = self.getDefaultsForValues(symbols=symbols, exclude=exclude)
		self.args.mergeDefaults(defaults)
		arguments = self.args.getValuesOrTypesAsDict(symbols=symbols, exclude=exclude)

		# Read the validation for the value. it comes in part from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		# TODO: check if it actually make sense to use entity.contracts instead of self.contracts
		validation = self._makeValueValidation(symbols=symbols, contracts=entity.contracts)
		if validation is not None:
			result = validation.validate(arguments, output="return")
			Error.assertTrue(element=self.element, attr="type", condition=bool(result), message=str(result))

	def _makeValueValidation(self, symbols: typing.Any, contracts: Contracts) -> typing.Optional[Validation]:
		"""
		Generate the validation for the value by combining the type validation
		and the contract validation.
		"""

		# The validation comes from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
		validationValue = contracts.validationForValue

		# Get the configuration value if any.
		if self.underlyingType is not None:
			underlyingType = symbols.getEntity(self.underlyingType)
			if underlyingType.isConfig:
				self.assertTrue(condition=not validationValue,
					message="Value-specific contracts cannot be associated with a configuration.")
				return self.makeValidationForValue(symbols=symbols)

		# If evaluates to true, meaning there is a contract for values,
		# it means there must be a single value.
		if validationValue:
			try:
				return Validation({"0": validationValue})
			except Exception as e:
				self.error(message=str(e))

		# Validation is empty
		return Validation({})

	@cached_property
	def args(self) -> Parameters:
		return Parameters(element=self.element, nestedKind="argument")

	@property
	def isArg(self) -> bool:
		return bool(self.args)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	def __repr__(self) -> str:
		return self.toString({
			"name": self.name if self.isName else "",
			"type": str(self.type) if self.isType else None,
			"value": str(self.value) if self.isValue else None
		})

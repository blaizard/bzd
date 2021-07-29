import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.entity import Entity, Role


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
	- Sequence:
		- argument: The list of arguments to pass to the instanciation or method call.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		self.assertTrue(condition=(self.isValue or self.isType), message="Expression must represent a type or a value.")

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name")

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

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve entities.
		"""
		entity = self.type.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

		# Generate this symbol FQN
		if entity.underlying is not None:
			self._setUnderlying(entity.underlying)

		# Resolve contract
		self.contracts.mergeBase(entity.contracts)

		# Add mandatory contract if the expression does not have default value.
		if not self.isArg:
			Contract.add(element=self.element, kind="mandatory")

		# Generate the argument list
		self.args.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		arguments = self.args.valuesAsDict

		print("arguments", arguments)

		# Read the validation for the value. it comes in part from the direct underlying type, contract information
		# directly associated with this expression do not apply to the current validation.
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
		if self.underlying is not None:
			underlying = symbols.getEntity(self.underlying)
			if underlying.isConfig:
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
		return self.toString({"name": self.name if self.isName else "", "type": str(self.type)})

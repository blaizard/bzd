import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.entity import Entity, Role


class Argument(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		Error.assertHasAttr(element=element, attr="value")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def value(self) -> str:
		return self.element.getAttr("value").value

	@property
	def key(self) -> typing.Optional[str]:
		return self.element.getAttrValue("name")


class Expression(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		Error.assertHasAttr(element=element, attr="type")

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name")

	@property
	def category(self) -> str:
		return "expression"

	@property
	def const(self) -> bool:
		return self.element.isAttr("const")

	@cached_property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", template="template")

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve entities.
		"""
		entity = self.type.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

		# Generate this symbol FQN
		self._setUnderlying(entity.underlying)

		# Resolve contract
		self.contracts.mergeBase(entity.contracts)

		print("expression", self.name, self.contracts)

		# Generate the argument list
		arguments = {(str(i) if arg.key is None else arg.key): arg.value for i, arg in enumerate(self.args)}

		# Read the validation for the value
		validation = self._makeValueValidation(entity=entity)
		if validation is not None:
			result = validation.validate(arguments, output="return")
			Error.assertTrue(element=self.element, condition=bool(result), message=str(result))

	def _makeValueValidation(self, entity: Entity) -> typing.Optional[Validation]:
		"""
		Generate the validation for the value by combining the type validation
		and the contract validation.
		"""

		# The validation comes from the underlying type, all contract information for this expression
		# do not apply to the current validation.
		validationValue = entity.contracts.validationForValue
		# If evaluates to true, meaning there is a contract for values,
		# it means there must be a single value.
		if validationValue:
			self.assertTrue(condition=not entity.isConfig,
				message="Value-specific contracts cannot be associated with a configuration.")
			try:
				return Validation({"0": validationValue})
			except Exception as e:
				self.error(message=str(e))

		# TODO fix this
		# Hack, need to resolve also this information
		#if entity.element.isNestedSequence("config"):
		#	return None

		return Validation({})

	@cached_property
	def args(self) -> typing.List[Argument]:
		arguments = self.element.getNestedSequence("argument")
		return [] if arguments is None else [Argument(arg) for arg in arguments]

	@property
	def isArg(self) -> bool:
		return len(self.args) > 0

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	def __repr__(self) -> str:
		return self.toString({"name": self.name if self.isName else "", "type": str(self.type)})

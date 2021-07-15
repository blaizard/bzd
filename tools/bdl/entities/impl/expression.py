import typing

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.utils.memoized_property import memoized_property
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.entity import Entity


class Argument(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)
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

		super().__init__(element)
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

	@memoized_property
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

		# Validate arguments
		if self.element.isNestedSequence("argument"):
			arguments = {(arg.getAttr("name").value if arg.isAttr("name") else str(i)): arg.getAttr("value").value
				for i, arg in enumerate(self.element.getNestedSequence("argument"))}  # type: ignore

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

		# Read the validation coming from the type if any
		validationType: typing.Optional[Validation] = entity.validation
		#validation = validationType if validationType else Validation({})

		contracts = entity.contracts
		print(contracts)

		# Check if there is a validation for the value
		#validationValue = self.contracts.validationValue
		#if validationValue:
		# Assert this is a single value
		#	try:
		#		validation.mergeSchema({"0": validationValue})
		#	except Exception as e:
		#		self.error(message=str(e))

		return validationType

	@memoized_property
	def args(self) -> typing.List[Argument]:
		arguments = self.element.getNestedSequence("argument")
		return [] if arguments is None else [Argument(arg) for arg in arguments]

	@property
	def isArg(self) -> bool:
		return len(self.args) > 0

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@memoized_property
	def contracts(self) -> Contracts:  # type: ignore
		return Contracts(element=self.element)

	def __repr__(self) -> str:
		return self.toString({"name": self.name if self.isName else "", "type": str(self.type)})

import typing

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.utils.memoized_property import memoized_property

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
			arguments = {(arg.getAttr("key").value if arg.isAttr("key") else str(i)): arg.getAttr("value").value
				for i, arg in enumerate(self.element.getNestedSequence("argument"))}  # type: ignore
			validation = entity.validation
			if validation is not None:
				result = validation.validate(arguments, output="return")
				Error.assertTrue(element=self.element, condition=result, message=str(result))
		

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
	def contracts(self) -> Contracts:
		return Contracts(sequence=self.element.getNestedSequence("contract"))

	def __repr__(self) -> str:
		return self.toString({"name": self.name if self.isName else "", "type": str(self.type)})

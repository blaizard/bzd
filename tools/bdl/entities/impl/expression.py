import typing

from bzd.parser.element import Element
from bzd.parser.error import Error

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

	@property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", template="template")

	def resolve(self, symbols: typing.Any, namespace: typing.List[str]) -> None:
		"""
		Resolve entities.
		"""
		self.type.resolve(symbols=symbols, namespace=namespace)

	@property
	def args(self) -> typing.List[Argument]:
		arguments = self.element.getNestedSequence("argument")
		return [] if arguments is None else [Argument(arg) for arg in arguments]

	@property
	def isArg(self) -> bool:
		return len(self.args) > 0

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def contracts(self) -> Contracts:
		return Contracts(sequence=self.element.getNestedSequence("contract"))

	def __repr__(self) -> str:
		return self.toString({"name": self.name if self.isName else "", "type": str(self.type)})

import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.entity import Entity, Role


class Using(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="type")

	@property
	def category(self) -> str:
		return "using"

	@property
	def contracts(self) -> Contracts:
		return self.type.contracts

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

		# Resolve contract
		self.contracts.mergeBase(entity.contracts)

		print("using", self.name, self.contracts)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

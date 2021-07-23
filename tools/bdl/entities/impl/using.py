import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
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

		# Resolve config if any
		#if entity.isConfig:
		#	config = entity.element.getNestedSequenceAssert("config")
		#	self.assertTrue(condition=not self.isConfig or self.element.getNestedSequence("config") == config, message="Conflict with exsiting configuration.")
		#	ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("config", config)
		# TODO: add the new symbols to the symbol map
		# and use reference to refer to it.
		# Example: MyNewType.value: <sequence config><element ref="OriginalType.value">

		print("using", self.name, self.contracts)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	def __repr__(self) -> str:
		return self.toString({"name": self.name})

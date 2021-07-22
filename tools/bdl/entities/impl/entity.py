import typing
import json
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.contract import Contracts


class Role:
	Undefined: int = 0
	# Represents a value.
	Value: int = 1
	# Represents a type.
	Type: int = 2
	# Represents a meta expression, something used only during the build phase.
	Meta: int = 4


class Entity:

	def __init__(self, element: Element, role: int) -> None:
		self.element = element
		self.role = role

	def _getNestedByCategory(self, category: str) -> typing.Any:
		sequence = self.element.getNestedSequence(category)
		if sequence:
			from tools.bdl.entities.all import elementToEntity
			return [elementToEntity(element) for element in sequence]
		return []

	@property
	def isRoleValue(self) -> bool:
		return bool(self.role & Role.Value)

	@property
	def isRoleType(self) -> bool:
		return bool(self.role & Role.Type)

	@property
	def isRoleMeta(self) -> bool:
		return bool(self.role & Role.Meta)

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name")

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	@property
	def contracts(self) -> Contracts:
		return Contracts(element=self.element)

	@cached_property
	def validationTemplate(self) -> typing.Optional[Validation]:
		schema = [
			config.contracts.validationForTemplate for config in self._getNestedByCategory("config")
			if config.contracts.get("template")
		]
		if schema:
			print(schema)
			return Validation(schema=["" if e is None else e for e in schema])
		return None

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve the current symbol.
		"""
		pass

	def error(self, message: str) -> None:
		Error.handleFromElement(element=self.element, message=message)

	def assertTrue(self, condition: bool, message: str) -> None:
		if not condition:
			self.error(message=message)

	def toString(self, attrs: typing.MutableMapping[str, str] = {}) -> str:
		entities = ["{}=\"{}\"".format(key, value) for key, value in attrs.items() if value]
		return "<{}/>".format(" ".join([type(self).__name__] + entities))

	def __repr__(self) -> str:
		"""
		Human readable string representation of a result.
		"""
		return self.toString()

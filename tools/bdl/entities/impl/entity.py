import typing
import json
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.contract import Contracts


class Entity:

	def __init__(self, element: Element) -> None:
		self.element = element

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
	def configActual(self) -> typing.List[typing.Any]:
		# TODO should be merged with config
		from tools.bdl.entities.impl.expression import Expression
		sequence = self.element.getNestedSequence("config")
		if sequence:
			config = []
			for element in sequence:
				if element.getAttr("category").value == "expression":
					config.append(Expression(element))
			return config
			#print(sequence)
			#return [Expression(element=element) for element in sequence]
		return []

	@cached_property
	def validationTemplate(self) -> typing.Optional[Validation]:
		schema = [
			config.contracts.validationForValue for config in self.configActual if config.contracts.get("template")
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

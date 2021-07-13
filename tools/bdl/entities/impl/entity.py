import typing
import json

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.validation.validation import Validation
from bzd.utils.memoized_property import memoized_property

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
		return Contracts(sequence = None)

	@memoized_property
	def validation(self) -> typing.Optional[Validation]:
		schema = self.element.getAttrValue("validation")
		if schema is None:
			return None
		return Validation(schema=json.loads(schema))

	@memoized_property
	def validationTemplate(self) -> typing.Optional[Validation]:
		schema = self.element.getAttrValue("validation_template")
		if schema is None:
			return None
		return Validation(schema=json.loads(schema))

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

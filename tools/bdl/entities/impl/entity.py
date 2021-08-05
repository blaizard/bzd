import typing
import json
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from tools.bdl.contracts.validation import Validation
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters, ResolvedType

if typing.TYPE_CHECKING:
	from bdl.entities.impl.expression import Expression


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

	def _setUnderlyingType(self, fqn: str) -> None:
		ElementBuilder.cast(self.element, ElementBuilder).addAttr("fqn_type", fqn)

	@property
	def underlyingType(self) -> typing.Optional[str]:
		"""
		Get the underlying element FQN if available.
		"""
		return self.element.getAttrValue("fqn_type")

	def _setUnderlyingValue(self, entity: "Entity", fqn: typing.Optional[str] = None) -> None:
		elementBuilder = ElementBuilder.cast(self.element, ElementBuilder)
		if fqn is not None:
			elementBuilder.addAttr("fqn_value", fqn)
		elif entity.underlyingValue is not None:
			elementBuilder.addAttr("fqn_value", entity.underlyingValue)
		if entity.literal is not None:
			elementBuilder.addAttr("literal", entity.literal)

	@property
	def underlyingValue(self) -> typing.Optional[str]:
		"""
		Get the underlying element FQN that contains the value.
		"""
		return self.element.getAttrValue("fqn_value")

	@property
	def literal(self) -> typing.Optional[str]:
		"""
		Get the underlying literal value if any.
		"""
		return self.element.getAttrValue("literal")

	@property
	def isNested(self) -> bool:
		return self.element.isNestedSequence("nested")

	@property
	def isConfig(self) -> bool:
		return self.element.isNestedSequence("config")

	@property
	def isComposition(self) -> bool:
		return self.element.isNestedSequence("composition")

	@property
	def nested(self) -> typing.Any:
		return self._getNestedByCategory("nested")

	@property
	def config(self) -> typing.Any:
		return self._getNestedByCategory("config")

	@property
	def composition(self) -> typing.Any:
		return self._getNestedByCategory("composition")

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

	def getConfigTemplates(self, symbols: typing.Any) -> typing.List["Expression"]:
		"""
		Get the list of expressions that forms the template.
		"""

		if self.underlyingType:
			underlyingType = symbols.getEntity(self.underlyingType)
			return [config for config in underlyingType.config if config.contracts.get("template")]
		return []

	def getConfigValues(self, symbols: typing.Any) -> typing.List["Expression"]:
		"""
		Get the list of expressions that forms the values.
		"""

		if self.underlyingType:
			underlyingType = symbols.getEntity(self.underlyingType)
			return [config for config in underlyingType.config if not config.contracts.get("template")]
		return []

	def makeValidationForTemplate(self, symbols: typing.Any) -> typing.Optional[Validation]:
		"""
		Generate the validation object for template parameters.
		"""
		schema = self.getConfigTemplates(symbols=symbols)
		if schema:
			try:
				return Validation(schema=[
					"" if e.contracts.validationForTemplate is None else e.contracts.validationForTemplate
					for e in schema
				])
			except Exception as e:
				self.error(message=str(e))
		return None

	def getDefaultsForTemplate(self, symbols: typing.Any, exclude: typing.Optional[typing.List[str]]) -> Parameters:
		"""
		Get the default values for the template.
		"""
		if self.underlyingType:
			underlyingType = symbols.getEntityAssert(fqn=self.underlyingType, element=self.element)
			return Parameters(element=underlyingType.element,
				nestedKind="config",
				filterFct=lambda entity: entity.contracts.has("template"))
		return Parameters(element=self.element)

	def makeValidationForValue(self, symbols: typing.Any) -> typing.Optional[Validation]:
		"""
		Generate the validation object for value parameters.
		"""
		schema = {}
		for index, expression in enumerate(self.getConfigValues(symbols=symbols)):
			schema[expression.name if expression.isName else str(index)] = expression.contracts.validationForValue
		if schema:
			try:
				return Validation(schema=schema)
			except Exception as e:
				self.error(message=str(e))
		return None

	def getDefaultsForValues(self, symbols: typing.Any, exclude: typing.Optional[typing.List[str]]) -> Parameters:
		"""
		Get the default values for the values.
		"""
		if self.underlyingType:
			underlyingType = symbols.getEntityAssert(fqn=self.underlyingType, element=self.element)
			return Parameters(element=underlyingType.element,
				nestedKind="config",
				filterFct=lambda entity: not entity.contracts.has("template"))
		return Parameters(element=self.element)

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

	def toString(self, attrs: typing.MutableMapping[str, typing.Optional[str]] = {}) -> str:
		entities = ["{}=\"{}\"".format(key, value) for key, value in attrs.items() if value]
		return "<{}/>".format(" ".join([type(self).__name__] + entities))

	def __repr__(self) -> str:
		"""
		Human readable string representation of a result.
		"""
		return self.toString()

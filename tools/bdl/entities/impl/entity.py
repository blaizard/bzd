import typing
import json
import copy
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from tools.bdl.contracts.validation import Validation, SchemaDict
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters, ResolvedType
from tools.bdl.entities.impl.fragment.sequence import EntitySequence
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.fragment.type import Type

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.entities.impl.using import Using
	from tools.bdl.entities.all import EntityType
	from tools.bdl.visitors.symbol_map import Resolver


class Role:
	Undefined: int = 0
	# Represents a value.
	Value: int = 1
	# Represents a type.
	Type: int = 2
	# Represents a meta expression, something used only during the build phase.
	Meta: int = 4


U = typing.TypeVar("U", bound="Entity")


class Entity:

	contractAttr: str = "contract"

	def __init__(self, element: Element, role: int) -> None:
		self.element = element
		self.role = role

	@property
	def category(self) -> str:
		return self.element.getAttrValue("category")

	def copy(self: U) -> U:
		"""
		Make a copy of the current entity.
		"""
		copySelf = copy.copy(self)
		copySelf.element = self.element.copy()
		return copySelf

	def toLiteral(self, args: typing.Any) -> typing.Optional[str]:
		"""Given the input parameters, generate the underlying literal of this object."""
		return None

	def _getNestedByCategory(self, category: str) -> EntitySequence:
		sequence = self.element.getNestedSequence(category)
		if sequence:
			from tools.bdl.entities.all import elementToEntity
			return EntitySequence([elementToEntity(element) for element in sequence])
		return EntitySequence([])

	def _setUnderlyingTypeFQN(self, fqn: str) -> None:
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("fqn_type", fqn)

	@property
	def configAttr(self) -> str:
		return "config"

	def getParents(self) -> typing.List[str]:
		"""
		Get the current entity direct parents.
		"""
		if self.element.isAttr("parents"):
			return self.element.getAttr("parents").value.split(";")
		return []

	def getUnderlyingTypeParents(self, resolver: "Resolver") -> typing.List[str]:
		"""
		Get the parents of the underlying type or the current entity if not present.
		"""
		if self.underlyingTypeFQN is None:
			return self.getParents()
		return self.getEntityUnderlyingTypeResolved(resolver=resolver).getParents()

	def addParents(self, fqn: typing.Optional[str], parents: typing.List[str]) -> None:
		"""
		Add parents to the current entity.
		"""
		if fqn is None:
			updatedParents = {*self.getParents(), *parents}
		else:
			updatedParents = {*self.getParents(), fqn, *parents}
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("parents", ";".join(updatedParents))

	@property
	def underlyingTypeFQN(self) -> typing.Optional[str]:
		"""
		Get the underlying element FQN if available.
		"""
		return self.element.getAttrValue("fqn_type")

	def _setUnderlyingValueFQN(self, fqn: str) -> None:
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("fqn_value", fqn)

	def _setLiteral(self, value: typing.Optional[str]) -> None:
		"""Set or unset the literal associated with this element."""

		if value is not None:
			ElementBuilder.cast(self.element, ElementBuilder).setAttr("literal", value)
		elif self.element.isAttr("literal"):
			ElementBuilder.cast(self.element, ElementBuilder).removeAttr("literal")

	@property
	def underlyingValueFQN(self) -> typing.Optional[str]:
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

	def getEntityUnderlyingTypeResolved(self, resolver: "Resolver") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		self.assertTrue(condition=self.underlyingTypeFQN is not None, message="Underlying type is not available.")
		assert self.underlyingTypeFQN is not None
		entity = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(element=self.element)
		assert entity.isRoleType, "The role of this entity must be a type."
		return entity

	@property
	def dependencies(self) -> typing.Set[str]:
		return set()

	@property
	def isInterface(self) -> bool:
		return self.element.isNestedSequence("interface")

	@property
	def isConfig(self) -> bool:
		return self.element.isNestedSequence(self.configAttr)

	@property
	def isConfigRaw(self) -> bool:
		return self.element.isNestedSequence("config")

	@property
	def isComposition(self) -> bool:
		return self.element.isNestedSequence("composition")

	@property
	def interface(self) -> EntitySequence:
		return self._getNestedByCategory("interface")

	@property
	def config(self) -> EntitySequence:
		return self._getNestedByCategory(self.configAttr)

	@property
	def configRaw(self) -> EntitySequence:
		return self._getNestedByCategory("config")

	@property
	def composition(self) -> EntitySequence:
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
		return Contracts(element=self.element, sequenceKind=self.contractAttr)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def isNamespace(self) -> bool:
		return self.isFQN

	@property
	def namespace(self) -> typing.List[str]:
		return FQN.toNamespace(self.fqn)[:-1]

	@property
	def isFQN(self) -> bool:
		return self.element.isAttr("fqn")

	@property
	def fqn(self) -> str:
		return self.element.getAttr("fqn").value

	@property
	def fqnToType(self) -> Type:
		return Type(element=self.element, kind="fqn")

	def getConfigTemplateTypes(self, resolver: typing.Any) -> Parameters:
		"""Get the list of expressions that forms the template types."""

		if self.underlyingTypeFQN:
			underlyingType = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(element=self.element)
			return Parameters(element=underlyingType.element,
				nestedKind=underlyingType.configAttr,
				filterFct=lambda entity: entity.category == "using")
		return Parameters(element=self.element)

	def getConfigValues(self, resolver: typing.Any) -> Parameters:
		"""
		Get the list of expressions that forms the values.
		"""

		if self.underlyingTypeFQN:
			underlyingTypeFQN = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(element=self.element)
			return Parameters(element=underlyingTypeFQN.element,
				nestedKind=underlyingTypeFQN.configAttr,
				filterFct=lambda entity: entity.category == "expression")
		return Parameters(element=self.element)

	def makeValidation(self, resolver: typing.Any, parameters: Parameters,
		forTemplate: bool) -> typing.Optional[Validation[SchemaDict]]:
		"""
		Generate the validation object.
		"""
		schema = {}
		for key, expression in parameters.items(includeVarArgs=True):
			key = "*" if expression.isVarArgs else str(key)
			maybeContracts = expression.contracts.validationForTemplate if forTemplate else expression.contracts.validationForValue
			schema[key] = maybeContracts if maybeContracts is not None else ""

		if schema:
			try:
				return Validation(schema=schema, args={"resolver": resolver})
			except Exception as e:
				self.error(message=str(e))
		return None

	def makeValidationForTemplate(self, resolver: typing.Any, parameters: Parameters) -> typing.Optional[
		Validation[SchemaDict]]:
		"""
		Generate the validation object for template parameters.
		"""
		return self.makeValidation(resolver=resolver, parameters=parameters, forTemplate=True)

	def makeValidationForValues(self, resolver: typing.Any, parameters: Parameters) -> typing.Optional[
		Validation[SchemaDict]]:
		"""
		Generate the validation object for value parameters.
		"""
		return self.makeValidation(resolver=resolver, parameters=parameters, forTemplate=False)

	def markAsResolved(self) -> None:
		"""
		Mark an entity as resolved.
		"""
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("resolved", "1")

	@property
	def isResolved(self) -> bool:
		return self.element.getAttrValue("resolved") == "1"

	def resolveMemoized(self, resolver: "Resolver") -> None:
		"""
		Resolve the current symbol.
		"""
		if self.isResolved:
			return
		self.resolve(resolver=resolver)
		self.markAsResolved()

	def resolve(self, resolver: "Resolver") -> None:
		"""Generic validation for all entities."""

		# Validate the contracts.
		maybeSchema = self.contracts.validationForEntity
		if maybeSchema:
			resultValidate = Validation(schema=[maybeSchema], args={
				"resolver": resolver
			}).validate([self], output="return")
			self.assertTrue(condition=bool(resultValidate), message=str(resultValidate))

		# Resolve the types from a config sequence if any.
		for entity in self.configRaw:
			if entity.category == "expression":
				# Only the type of the expression should be resolved, the value cannot be evaluated
				# as it might be malformed, this is inherent from config expressions.
				typing.cast("Expression", entity).type.resolve(resolver=resolver)
			elif entity.category == "using":
				typing.cast("Using", entity).resolve(resolver=resolver)
			else:
				self.error(f"Configuration can only contain expressions or using statements, not '{entity.category}'.")

	def error(self, message: str, throw: bool = True) -> str:
		return Error.handleFromElement(element=self.element, message=message, throw=throw)

	def assertTrue(self, condition: bool, message: str, throw: bool = True) -> typing.Optional[str]:
		return Error.assertTrue(condition=condition, element=self.element, message=message, throw=throw)

	def toString(self, attrs: typing.MutableMapping[str, typing.Optional[str]] = {}) -> str:
		entities = ["{}=\"{}\"".format(key, value) for key, value in attrs.items() if value]
		return "<{}/>".format(" ".join([type(self).__name__] + entities))

	def __repr__(self) -> str:
		"""
		Human readable string representation of a result.
		"""
		return self.toString()


class EntityExpression(Entity):

	def __init__(self, element: Element, role: int = Role.Undefined) -> None:
		self.element = element
		self.role = role

	@property
	def const(self) -> bool:
		return self.type.const

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name") and not self.name == "..."

	@property
	def isVarArgs(self) -> bool:
		return self.element.isAttr("name") and self.name == "..."

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@cached_property
	def type(self) -> Type:
		return Type(element=self.element, kind="type", underlyingTypeFQN="fqn_type", template="template", const="const")

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def value(self) -> str:
		return self.element.getAttr("value").value

	@cached_property
	def isParameters(self) -> bool:
		return self.element.isNestedSequence("argument")

	def __repr__(self) -> str:

		return self.toString({
			"name": self.name if self.isName else "",
			"type": str(self.type) if self.isType else None,
			"value": str(self.value) if self.isValue else None
		})

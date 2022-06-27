import typing
import json
import copy

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

	def copy(self: U) -> U:
		"""
		Make a copy of the current entity.
		"""
		copySelf = copy.copy(self)
		copySelf.element = self.element.copy()
		return copySelf

	def _getNestedByCategory(self, category: str) -> EntitySequence:
		sequence = self.element.getNestedSequence(category)
		if sequence:
			from tools.bdl.entities.all import elementToEntity
			return EntitySequence([elementToEntity(element) for element in sequence])
		return EntitySequence([])

	def _setUnderlyingType(self, fqn: str) -> None:
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
		if self.underlyingType is None:
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
	def underlyingType(self) -> typing.Optional[str]:
		"""
		Get the underlying element FQN if available.
		"""
		return self.element.getAttrValue("fqn_type")

	def _setUnderlyingValue(self, entity: "Entity", fqn: typing.Optional[str] = None) -> None:
		elementBuilder = ElementBuilder.cast(self.element, ElementBuilder)
		if fqn is not None:
			elementBuilder.setAttr("fqn_value", fqn)
		elif entity.underlyingValue is not None:
			elementBuilder.setAttr("fqn_value", entity.underlyingValue)
		if entity.literal is not None:
			elementBuilder.setAttr("literal", entity.literal)

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

	def getEntityUnderlyingTypeResolved(self, resolver: "Resolver") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		self.assertTrue(condition=self.underlyingType is not None, message="Underlying type is not available.")
		assert self.underlyingType is not None
		entity = resolver.getEntityResolved(fqn=self.underlyingType).assertValue(element=self.element)
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
		"""
		Get the list of expressions that forms the template types.
		"""

		if self.underlyingType:
			underlyingType = resolver.getEntityResolved(fqn=self.underlyingType).assertValue(element=self.element)
			return Parameters(element=underlyingType.element,
				nestedKind=underlyingType.configAttr,
				filterFct=lambda entity: entity.contracts.has("template") and entity.contracts.has("type"))
		return Parameters(element=self.element)

	def getConfigValues(self, resolver: typing.Any) -> Parameters:
		"""
		Get the list of expressions that forms the values.
		"""

		if self.underlyingType:
			underlyingType = resolver.getEntityResolved(fqn=self.underlyingType).assertValue(element=self.element)
			return Parameters(element=underlyingType.element,
				nestedKind=underlyingType.configAttr,
				filterFct=lambda entity: not (entity.contracts.has("template") and entity.contracts.has("type")))
		return Parameters(element=self.element)

	def makeValidation(self, resolver: typing.Any, parameters: Parameters,
		forTemplate: bool) -> typing.Optional[Validation[SchemaDict]]:
		"""
		Generate the validation object.
		"""
		schema = {}
		for key, expression in parameters.items():
			if expression.isVarArgs:
				key = "*"

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
		maybeSchema = self.contracts.validationForEntity
		if maybeSchema:
			resultValidate = Validation(schema=[maybeSchema], args={
				"resolver": resolver
			}).validate([self], output="return")
			self.assertTrue(condition=bool(resultValidate), message=str(resultValidate))

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

import typing
import json
import copy
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error, AssertionResult

from bdl.contracts.validation import Validation, SchemaDict
from bdl.entities.impl.fragment.contract import Contracts
from bdl.entities.impl.fragment.parameters import Parameters, ResolvedType
from bdl.entities.impl.fragment.sequence import EntitySequence
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem
from bdl.entities.impl.types import Category
from bdl.entities.impl.fragment.regexpr import Regexpr

if typing.TYPE_CHECKING:
	from bdl.entities.impl.expression import Expression
	from bdl.entities.impl.using import Using
	from bdl.entities.all import EntityType
	from bdl.visitors.symbol_map import Resolver


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
	def category(self) -> Category:
		return Category(self.element.getAttrValue("category"))

	def copy(self: U) -> U:
		"""Make a shallow copy of the current entity."""

		copySelf = copy.copy(self)
		copySelf.element = self.element.copy()
		return copySelf

	def deepCopy(self: U) -> U:
		"""Make a deep copy of the current entity."""

		copySelf = copy.copy(self)
		copySelf.element = self.element.deepCopy()
		return copySelf

	def toLiteral(self, args: typing.Dict[str, "EntityExpression"]) -> typing.Optional[str]:
		"""Given the input parameters, generate the underlying literal of this object."""
		return None

	def _getNestedByCategory(self, category: str) -> EntitySequence:
		sequence = self.element.getNestedSequence(category)
		if sequence:
			from bdl.entities.all import elementToEntity

			return EntitySequence([elementToEntity(element) for element in sequence])
		return EntitySequence([])

	def _setUnderlyingTypeFQN(self, fqn: str) -> None:
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("fqn_type", fqn)

	def _setMeta(self) -> None:
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("meta", "1")

	@property
	def configAttr(self) -> str:
		return "config"

	@property
	def interfaceAttr(self) -> str:
		return "interface"

	@property
	def compositionAttr(self) -> str:
		return "composition"

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
	def isOperator(self) -> bool:
		"""Check if it has an operator."""
		return self.element.isAttr("operator")

	@property
	def operator(self) -> str:
		return self.element.getAttr("operator").value

	@property
	def isRegexprAttr(self) -> bool:
		return self.element.isAttr("regexpr")

	@property
	def regexprAttr(self) -> str:
		return self.element.getAttr("regexpr").value

	@property
	def isRegexpr(self) -> bool:
		return Regexpr(self.element).isValid

	@property
	def regexpr(self) -> Regexpr:
		return Regexpr(self.element)

	def _setRegexpr(self, regexpr: typing.Optional[Regexpr]) -> None:
		"""Set or unset the regular expression associated with this element."""

		if regexpr is None:
			self.regexpr.clear()
		else:
			self.regexpr.assign(regexpr)

	@property
	def isPreset(self) -> bool:
		return self.element.isAttr("preset")

	@property
	def preset(self) -> str:
		"""Get the preset associated with this entity."""
		return self.element.getAttr("preset").value

	@property
	def isLiteral(self) -> bool:
		"""Check if a value has a literal underlying value."""
		return self.element.isAttr("literal")

	@property
	def literal(self) -> typing.Optional[str]:
		"""
        Get the underlying literal value if any.
        """
		return self.element.getAttrValue("literal")

	@property
	def literalNative(self) -> typing.Optional[typing.Union[int, float, str, bool]]:
		"""
        Get the underlying literal value if any within its native type.
        """

		literal = self.literal
		if literal is None:
			return None
		if literal == "true":
			return True
		if literal == "false":
			return False
		if literal.startswith('"'):
			assert literal.endswith(
			    '"'), f"If a literal starts with \", it also must end with \", instead received: '{literal}'."
			return literal[1:-1]
		converted = float(literal)
		if converted.is_integer():
			return int(converted)
		return converted

	def getEntityUnderlyingTypeResolved(self, resolver: "Resolver") -> "EntityType":
		"""Get the entity related to type after resolve."""

		self.assertTrue(
		    condition=self.underlyingTypeFQN is not None,
		    message="Underlying type is not available.",
		)
		assert self.underlyingTypeFQN is not None
		entity = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(element=self.element)
		assert entity.isRoleType, "The role of this entity must be a type."
		return entity

	@property
	def dependencies(self) -> typing.Set[str]:
		return set()

	@property
	def isExtern(self) -> bool:
		return self.element.isAttr("extern")

	@property
	def isInterface(self) -> bool:
		return self.element.isNestedSequence(self.interfaceAttr)

	@property
	def isConfig(self) -> bool:
		return self.element.isNestedSequence(self.configAttr)

	@property
	def isConfigRaw(self) -> bool:
		return self.element.isNestedSequence("config")

	@property
	def isComposition(self) -> bool:
		return self.element.isNestedSequence(self.compositionAttr)

	@property
	def interface(self) -> EntitySequence:
		return self._getNestedByCategory(self.interfaceAttr)

	@property
	def config(self) -> EntitySequence:
		return self._getNestedByCategory(self.configAttr)

	@property
	def configRaw(self) -> EntitySequence:
		return self._getNestedByCategory("config")

	@property
	def composition(self) -> EntitySequence:
		return self._getNestedByCategory(self.compositionAttr)

	@property
	def isRoleValue(self) -> bool:
		return bool(self.role & Role.Value)

	@property
	def isRoleType(self) -> bool:
		return bool(self.role & Role.Type)

	@property
	def isRoleMeta(self) -> bool:
		return bool(self.role & Role.Meta) or self.element.isAttr("meta")

	@property
	def isName(self) -> bool:
		return self.element.isAttr("name")

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value.replace("...", "")

	@property
	def contracts(self) -> Contracts:
		return Contracts(element=self.element, sequenceKind=self.contractAttr)

	@property
	def isExecutor(self) -> bool:
		"""If this element is an executor, return True."""
		maybeExecutor = self.contracts.get("executor")
		return maybeExecutor is not None and maybeExecutor.isValue == False

	@property
	def executor(self) -> typing.Optional[str]:
		"""Get the executor directly associated with this entity."""

		maybeExecutor = self.contracts.get("executor")
		if maybeExecutor is None:
			return None
		if maybeExecutor.isValue:
			return maybeExecutor.value
		return self.fqn

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

	def getConfigTemplateTypes(self, resolver: typing.Any) -> Parameters:
		"""Get the list of expressions that forms the template types."""

		from bdl.entities.impl.using import Using

		if self.underlyingTypeFQN:
			underlyingType = resolver.getEntityResolved(fqn=self.underlyingTypeFQN).assertValue(element=self.element)
			params = Parameters(
			    element=underlyingType.element,
			    NestedElementType=Using,
			    nestedKind=underlyingType.configAttr,
			    filterFct=lambda entity: entity.category == Category.using,
			)
			# Resolve the parameters only when used.
			for entity in params:
				typing.cast("Using", entity).resolveMemoized(resolver=resolver)
			return params

		return Parameters(element=self.element, NestedElementType=Using)

	def markAsResolved(self) -> None:
		"""
        Mark an entity as resolved.
        """
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("resolved", "1")

	@property
	def isResolved(self) -> bool:
		return self.element.getAttrValue("resolved") == "1"

	def resolveMemoized(self, resolver: "Resolver") -> None:
		"""Resolve the current symbol.

        Args:
                - resolver: The resolver for this entity.
        """

		if self.isResolved:
			return
		self.resolve(resolver=resolver)
		if resolver.memoize:
			self.markAsResolved()

	def resolve(self, resolver: "Resolver") -> None:
		"""Generic validation for all entities.

        Args:
                - resolver: The resolver for this entity.
        """

		# Validate the contracts.
		maybeSchema = self.contracts.validationForEntity
		if maybeSchema:
			resultValidate = Validation(schema=[maybeSchema], args={
			    "resolver": resolver
			}).validate([self], output="return")
			self.assertTrue(condition=bool(resultValidate), message=str(resultValidate))

		for entity in self.configRaw:
			self.assertTrue(
			    condition=entity.category in {Category.expression, Category.using},
			    message=f"Configuration can only contain expressions or using statements, not '{entity.category}'.",
			)
			# Note, config entities are resolved only later, when used.
			# This allow symbol discovery at a later stage, only when the element is actually instanciated.

	def error(self, message: str, element: typing.Optional[Element] = None, throw: bool = True) -> AssertionResult:
		return Error.handleFromElement(
		    element=self.element if element is None else element,
		    message=message,
		    throw=throw,
		)

	def assertTrue(
	    self,
	    condition: bool,
	    message: str,
	    element: typing.Optional[Element] = None,
	    throw: bool = True,
	) -> AssertionResult:
		return Error.assertTrue(
		    condition=condition,
		    element=self.element if element is None else element,
		    message=message,
		    throw=throw,
		)

	def toString(self, attrs: typing.MutableMapping[str, typing.Optional[str]] = {}) -> str:
		entities = ['{}="{}"'.format(key, value) for key, value in attrs.items() if value]
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
		return self.symbol.const

	@property
	def isVarArgs(self) -> bool:
		return self.element.isAttr("name") and self.element.getAttr("name").value.endswith("...")

	@property
	def isSymbol(self) -> bool:
		return self.element.isAttr("symbol")

	@property
	def symbol(self) -> Symbol:
		return Symbol(
		    element=self.element,
		    kind="symbol",
		    underlyingTypeFQN="fqn_type",
		    template="template",
		    const="const",
		)

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def value(self) -> str:
		return self.element.getAttr("value").value

	@property
	def isParameters(self) -> bool:
		return self.element.isNestedSequence("argument")

	@property
	def isRValue(self) -> bool:
		"""If the expression represents an RValue, in this context an rvalue is a temporary,
        but it could be extended to move semantic."""

		return self.isRoleValue and self.underlyingValueFQN is None

	@property
	def isLValue(self) -> bool:
		"""If the expression represents an LValue, in this context an lvalue is
        a reference to another value."""

		return (self.isRoleValue and self.isSymbol and self.underlyingValueFQN is not None)

	def toParametersResolvedItem(self, name: typing.Optional[str] = None) -> ParametersResolvedItem:
		"""Create an item from this expression.

        Args:
                name: Expected name.
        """
		element = ElementBuilder.cast(self.element.copy(), ElementBuilder)
		if name is not None:
			element.setAttr("name", name)
		else:
			element.removeAttr("name")
		expected = EntityExpression(element=element, role=self.role)
		return ParametersResolvedItem(param=self, expected=expected)

	@property
	def underlyingInterfaceFQN(self) -> typing.Optional[str]:
		"""Get the underlying element interface FQN if available."""

		return self.element.getAttrValue("fqn_interface")

	def __repr__(self) -> str:
		return self.toString({
		    "name": self.name if self.isName else "",
		    "varArgs": "true" if self.isVarArgs else None,
		    "symbol": str(self.symbol) if self.isSymbol else None,
		    "value": str(self.value) if self.isValue else None,
		    "operator": str(self.operator) if self.isOperator else None,
		    "regexpr": str(self.regexprAttr) if self.isRegexprAttr else None,
		    "parameters": "[...]" if self.isParameters else None,
		})

import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.visitor import VisitorDepthFirst as VisitorDepthFirstBase
from bzd.parser.error import Error, AssertionResult

from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from tools.bdl.entities.impl.types import Category

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import EntityType
	from tools.bdl.visitors.symbol_map import Resolver


class Symbol:

	def __init__(self,
	             element: Element,
	             kind: str,
	             underlyingTypeFQN: typing.Optional[str] = None,
	             template: typing.Optional[str] = None,
	             contract: typing.Optional[str] = None,
	             const: typing.Optional[str] = None) -> None:

		Error.assertHasAttr(element=element, attr=kind)
		self.element = element
		self.kindAttr = kind
		self.underlyingTypeAttr = underlyingTypeFQN
		self.templateAttr = template
		self.contractAttr = contract
		self.constAttr = const

	@property
	def underlyingTypeFQN(self) -> typing.Optional[str]:
		if self.underlyingTypeAttr is None:
			return None
		return self.element.getAttrValue(self.underlyingTypeAttr)

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this type.
		"""
		dependencies = {*self.kinds}
		for params in self.templates:
			dependencies.update(params.dependencies)

		return dependencies

	def resolve(self, resolver: "Resolver", maybeValue: bool = False) -> "EntityType":
		"""Resolve the types and nested templates by updating their symbol to fqn.

		Args:
			- maybeValue: Set to true if the type might represent a value.
		"""

		# Make the fully qualified kind name.
		fqns = resolver.resolveFQN(name=self.fqn).assertValue(element=self.element, attr=self.kindAttr)
		ElementBuilder.cast(self.element, ElementBuilder).setAttr(f"{self.kindAttr}_resolved", ";".join(fqns))

		# Resolve the templates if available
		self.templates.resolve(resolver=resolver)

		# Get the symbol entity and save it as the underlying type
		entity = self.getEntityResolved(resolver=resolver)
		self.assertTrue(condition=entity.isRoleType or maybeValue, message="This is not a valid type.")

		# Resolve the entity, this is needed if the entity is not discovered yet.
		# It might happens for entities that are defined in a config section for example, which are only
		# resolved when used.
		entity.resolveMemoized(resolver=resolver.make(namespace=self.namespace))

		# Save the category under {kindAttr}_category.
		ElementBuilder.cast(self.element, ElementBuilder).setAttr(f"{self.kindAttr}_category", entity.category.value)

		if self.underlyingTypeAttr is not None and entity.underlyingTypeFQN is not None:
			ElementBuilder.cast(self.element, ElementBuilder).setAttr(self.underlyingTypeAttr, entity.underlyingTypeFQN)

		# Validate template arguments
		configTypes = entity.getConfigTemplateTypes(resolver=resolver)
		if not configTypes:
			self.assertTrue(condition=(not bool(self.templates)),
			                message=f"Symbol '{self.kind}' does not support template type arguments.")
		else:
			assert isinstance(self.templateAttr, str)
			self.templates.makeParametersResolved(name=self.templateAttr, resolver=resolver, expected=configTypes)

			# Make sure none of the template arguments have a meta role.
			for item in self.templateResolved:
				item.assertTrue(condition=not item.param.isRoleMeta,
				                message="Template arguments cannot have a 'meta' role.")

			# Validate the template arguments
			validation = configTypes.makeValidationForTemplate(resolver=resolver)
			arguments = self.templates.getValuesOrTypesAsDict(resolver=resolver, varArgs=False)
			result = validation.validate(arguments, output="return")
			self.assertTrue(condition=bool(result), message=str(result))

		# Resolve contract
		self.contracts.resolve(entity.contracts)

		return entity

	def getEntityResolved(self, resolver: "Resolver") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		return resolver.getEntityResolved(fqn=self.kind).assertValue(element=self.element, attr=self.kindAttr)

	def getEntityUnderlyingTypeResolved(self, resolver: "Resolver") -> "EntityType":
		self.assertTrue(condition=self.underlyingTypeFQN is not None,
		                message=f"The underlying type FQN is missing, {self.element}")
		assert isinstance(self.underlyingTypeFQN, str)
		return resolver.getEntity(fqn=self.underlyingTypeFQN).assertValue(element=self.element, attr=self.kindAttr)

	def getThisResolved(self, resolver: "Resolver") -> "EntityType":
		return resolver.getEntity(fqn=self.this).assertValue(element=self.element, attr=self.kindAttr)

	@property
	def const(self) -> bool:
		if self.constAttr is None:
			return False
		return self.element.isAttr(self.constAttr)

	@property
	def contracts(self) -> Contracts:
		return Contracts(element=self.element, sequenceKind=self.contractAttr)

	@property
	def isTemplate(self) -> bool:
		return bool(self.templates)

	@property
	def templates(self) -> "Parameters":
		from tools.bdl.entities.impl.using import Using
		return Parameters(element=self.element, NestedElementType=Using, nestedKind=self.templateAttr)

	@property
	def templateResolved(self) -> ParametersResolved:
		from tools.bdl.entities.impl.using import Using
		return ParametersResolved(element=self.element,
		                          NestedElementType=Using,
		                          param=f"{self.templateAttr}_resolved",
		                          expected=f"{self.templateAttr}_expected")

	@property
	def category(self) -> Category:
		return Category(self.element.getAttr(f"{self.kindAttr}_category").value)

	@property
	def fqn(self) -> str:
		return self.element.getAttr(self.kindAttr).value

	@property
	def isThis(self) -> bool:
		return len(self.kinds) > 1

	@property
	def this(self) -> str:
		fqn = [self.kinds[0]] + [kind.split(".")[-1] for kind in self.kinds[1:-1]]
		return ".".join(fqn)

	@property
	def propertyName(self) -> str:
		return self.kinds[-1].split(".")[-1]

	@property
	def kind(self) -> str:
		return self.kinds[-1]

	@property
	def kinds(self) -> typing.List[str]:
		return self.element.getAttr(f"{self.kindAttr}_resolved").value.split(";") if self.element.isAttr(
		    f"{self.kindAttr}_resolved") else [self.fqn]

	@property
	def namespace(self) -> typing.List[str]:
		"""Namespace associated with this symbol."""

		return self.kind.split(".")[:-1]

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def name(self) -> str:
		return Visitor(symbol=self).result

	def isConvertible(self, resolver: "Resolver", to: "Symbol") -> bool:
		"""Check that the current type is convertible to another type."""

		typeFrom = self.getEntityUnderlyingTypeResolved(resolver=resolver)
		typeTo = to.getEntityUnderlyingTypeResolved(resolver=resolver)

		if typeFrom == typeTo:
			return True

		# Check if it is convertible to any of its parent.
		for parentFQN in typeFrom.getUnderlyingTypeParents(resolver=resolver):
			entity = resolver.getEntityResolved(fqn=parentFQN).assertValue(element=typeFrom.element)
			if entity == typeTo:
				return True

		return False

	def error(self, message: str, throw: bool = True) -> AssertionResult:
		return Error.handleFromElement(element=self.element, attr=self.kindAttr, message=message, throw=throw)

	def assertTrue(self, condition: bool, message: str, throw: bool = True) -> AssertionResult:
		return Error.assertTrue(condition=condition,
		                        element=self.element,
		                        attr=self.kindAttr,
		                        message=message,
		                        throw=throw)

	def __repr__(self) -> str:
		return self.name

	def __eq__(self, other: "Symbol") -> bool:  # type: ignore
		return str(self) == str(other)

	def __ne__(self, other: "Symbol") -> bool:  # type: ignore
		return not (self == other)

	def __hash__(self) -> int:
		return hash(str(self))


class Visitor(VisitorDepthFirstBase[typing.List[str], str]):

	nestedKind = "template"

	def __init__(self, symbol: Symbol) -> None:

		# Nested level
		self.level = 0

		# Construct the template if any.
		if symbol.templateAttr is not None:
			self.nestedKind = symbol.templateAttr
			nested = symbol.element.getNestedSequence(self.nestedKind)
			template = [] if nested is None else self._visit(sequence=nested)
		else:
			template = []

		self.result = self.visitSymbol(symbol=symbol, nested=template, parameters=symbol.templateResolved)

	@property
	def isResolved(self) -> bool:
		return self.nestedKind.endswith("_resolved")

	@property
	def isTopLevel(self) -> bool:
		return self.level == 0

	def _visit(self, sequence: Sequence) -> typing.List[str]:
		self.level += 1
		result = super()._visit(sequence=sequence)
		self.level -= 1
		return result

	def visitBegin(self) -> typing.List[str]:
		return []

	def visitElement(self, element: Element, result: typing.List[str],
	                 nested: typing.Optional[typing.List[str]]) -> typing.List[str]:

		if element.isAttr("symbol"):

			symbol = Symbol(element=element,
			                kind="symbol",
			                underlyingTypeFQN="fqn_type",
			                template="template_resolved" if self.isResolved else "template",
			                const="const")
			output = self.visitSymbol(symbol=symbol,
			                          nested=[] if nested is None else nested,
			                          parameters=symbol.templateResolved)

		else:
			Error.assertHasAttr(element=element, attr="value")
			Error.assertTrue(element=element, condition=not nested, message="Value cannot have nested entities.")
			output = self.visitValue(value=element.getAttr("value").value, comment=element.getAttrValue("comment"))

		result.append(output)

		return result

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		return value

	def visitSymbol(self, symbol: Symbol, nested: typing.List[str], parameters: ParametersResolved) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		# Build the type. The first fqn is the element and the rest
		# are the types.
		kinds: typing.List[str] = []
		for index, fqn in enumerate(symbol.kinds):
			if index == 0:
				kinds.append(fqn)
			else:
				kinds.append(fqn.split(".")[-1])
		kindStr = ".".join(kinds)

		# If const
		if symbol.const:
			kindStr = f"const {kindStr}"

		if nested:
			return f"{kindStr}<{','.join(nested)}>"
		return kindStr

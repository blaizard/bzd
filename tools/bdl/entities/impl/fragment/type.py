import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.visitor import VisitorDepthFirst as VisitorDepthFirstBase
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import EntityType
	from tools.bdl.visitors.symbol_map import Resolver


class Type:

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
			maybeValue: Set to true if the type might represent a value.
		"""

		fqns = resolver.resolveFQN(name=self.kind).assertValue(element=self.element, attr=self.kindAttr)

		ElementBuilder.cast(self.element, ElementBuilder).updateAttr(self.kindAttr, ";".join(fqns))

		# Resolve the templates if available
		self.templates.resolve(resolver=resolver)

		# Get and save the underlying type
		underlying = self.getEntityResolved(resolver=resolver)
		self.assertTrue(condition=underlying.isRoleType or maybeValue, message="This is not a valid type.")
		# Resolve the entity, this is needed only if the entity is defined after the one holding this type.
		underlying.resolveMemoized(resolver=resolver)

		# Save the category under {kindAttr}_category.
		ElementBuilder.cast(self.element, ElementBuilder).setAttr(f"{self.kindAttr}_category", underlying.category)

		if self.underlyingTypeAttr is not None and underlying.underlyingTypeFQN is not None:
			ElementBuilder.cast(self.element, ElementBuilder).setAttr(self.underlyingTypeAttr,
				underlying.underlyingTypeFQN)

		# Validate template arguments
		configTypes = underlying.getConfigTemplateTypes(resolver=resolver)
		if not configTypes:
			self.assertTrue(condition=(not bool(self.templates)),
				message=f"Type '{self.kind}' does not support template type arguments.")
		else:

			self.templates.makeParametersResolved(name=self.templateAttr, resolver=resolver, expected=configTypes)

			# Make sure none of the template arguments have a meta role.
			for item in self.templateResolved:
				item.assertTrue(condition=not item.param.isRoleMeta, message="Template arguments cannot have a 'meta' role.")

			# Validate the template arguments
			validation = underlying.makeValidationForTemplate(resolver=resolver, parameters=configTypes)
			arguments = self.templates.getValuesOrTypesAsDict(resolver=resolver, varArgs=False)
			result = validation.validate(arguments, output="return")
			self.assertTrue(condition=bool(result), message=str(result))

		# Resolve contract
		self.contracts.resolve(underlying.contracts)

		return underlying

	def getEntityResolved(self, resolver: "Resolver") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		return resolver.getEntity(fqn=self.kind).assertValue(element=self.element, attr=self.kindAttr)

	def getEntityUnderlyingTypeResolved(self, resolver: "Resolver") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		entity = self.getEntityResolved(resolver=resolver)
		return entity.getEntityUnderlyingTypeResolved(resolver=resolver)

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

	@cached_property
	def templates(self) -> "Parameters":
		from tools.bdl.entities.impl.using import Using
		return Parameters(element=self.element, NestedElementType=Using, nestedKind=self.templateAttr)

	@cached_property
	def templateResolved(self) -> ParametersResolved:
		from tools.bdl.entities.impl.using import Using
		return ParametersResolved(element=self.element, NestedElementType=Using, param=f"{self.templateAttr}_resolved", expected=f"{self.templateAttr}_expected")

	@property
	def category(self) -> str:
		return self.element.getAttr(f"{self.kindAttr}_category").value

	@property
	def fqn(self) -> str:
		return self.kinds[-1]

	@property
	def kind(self) -> str:
		return self.fqn

	@property
	def kinds(self) -> typing.List[str]:
		return self.element.getAttr(self.kindAttr).value.split(";")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@cached_property
	def name(self) -> str:
		return Visitor(entity=self).result

	def isConvertible(self, resolver: "Resolver", to: "Type") -> bool:
		"""Check that the current type is convertible to another type."""

		typeFrom = self.getEntityUnderlyingTypeResolved(resolver=resolver)
		typeTo = to.getEntityUnderlyingTypeResolved(resolver=resolver)

		if typeFrom == typeTo:
			return True

		# Check if it is convertible to any of its parent.
		for parentFQN in typeFrom.getUnderlyingTypeParents(resolver=resolver):
			entity = resolver.getEntityResolved(fqn=parentFQN).assertValue(element=typeFrom)
			if entity == typeTo:
				return True

		return False

	def error(self, message: str, throw: bool = True) -> str:
		return Error.handleFromElement(element=self.element, attr=self.kindAttr, message=message, throw=throw)

	def assertTrue(self, condition: bool, message: str, throw: bool = True) -> typing.Optional[str]:
		return Error.assertTrue(condition=condition, element=self.element, attr=self.kindAttr, message=message, throw=throw)

	def __repr__(self) -> str:
		return self.name


class Visitor(VisitorDepthFirstBase[typing.List[str], str]):

	nestedKind = "template"

	def __init__(self, entity: Type) -> None:

		# Nested level
		self.level = 0

		# Construct the template if any.
		if entity.templateAttr is not None:
			self.nestedKind = entity.templateAttr
			nested = entity.element.getNestedSequence(self.nestedKind)
			template = [] if nested is None else self._visit(sequence=nested)
		else:
			template = []

		self.result = self.visitType(entity=entity, nested=template, parameters=entity.templateResolved)

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

		if element.isAttr("type"):

			entity = Type(element=element,
				kind="type",
				underlyingTypeFQN="fqn_type",
				template="template_resolved" if self.isResolved else "template",
				const="const")
			output = self.visitType(entity=entity,
				nested=[] if nested is None else nested,
				parameters=entity.templateResolved)

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

	def visitType(self, entity: Type, nested: typing.List[str], parameters: ParametersResolved) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		if nested:
			return "{}<{}>".format(entity.kind, ",".join(nested))
		return entity.kind

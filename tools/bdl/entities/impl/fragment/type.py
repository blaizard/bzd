import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.visitor import VisitorDepthFirst as VisitorDepthFirstBase
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import EntityType
	from tools.bdl.visitors.symbol_map import SymbolMap


class Type:

	def __init__(self,
		element: Element,
		kind: str,
		underlyingType: typing.Optional[str] = None,
		template: typing.Optional[str] = None) -> None:

		Error.assertHasAttr(element=element, attr=kind)
		self.element = element
		self.kindAttr = kind
		self.underlyingTypeAttr = underlyingType
		self.templateAttr = template

	@property
	def underlyingType(self) -> typing.Optional[str]:
		if self.underlyingTypeAttr is None:
			return None
		return self.element.getAttrValue(self.underlyingTypeAttr)

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this type.
		"""
		dependencies = set(self.kind)
		for params in self.templates:
			dependencies.update(params.dependencies)

		return dependencies

	def resolve(self,
		symbols: "SymbolMap",
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> "EntityType":
		"""
		Resolve the types and nested templates by updating their symbol to fqn.
		"""

		fqn = symbols.resolveFQN(name=self.kind, namespace=namespace, exclude=exclude).assertValue(element=self.element,
			attr=self.kindAttr)
		ElementBuilder.cast(self.element, ElementBuilder).updateAttr(self.kindAttr, fqn)

		# Resolve the templates if available
		self.templates.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		templates = self.templates

		# Get and save the underlying type
		underlying = self.getEntityResolved(symbols=symbols)
		if self.underlyingTypeAttr is not None and underlying.underlyingType is not None:
			ElementBuilder.cast(self.element, ElementBuilder).setAttr(self.underlyingTypeAttr,
				underlying.underlyingType)

		# Validate template arguments
		validation = underlying.makeValidationForTemplate(symbols=symbols)
		if validation is None:
			Error.assertTrue(element=self.element,
				condition=(not bool(self.templates)),
				attr=self.kindAttr,
				message="Type '{}' does not support template arguments.".format(self.kind))
		else:
			defaults = underlying.getDefaultsForTemplate(symbols=symbols, exclude=exclude)
			templates.mergeDefaults(defaults)

			# Save the resolved template
			sequence = templates.toResolvedSequence(symbols=symbols, exclude=exclude)
			ElementBuilder.cast(self.element, ElementBuilder).setNestedSequence("{}_resolved".format(self.templateAttr),
				sequence)

			# Validate the template arguments
			values = templates.getValuesOrTypesAsDict(symbols=symbols, exclude=exclude)
			resultValidate = validation.validate(values, output="return")
			Error.assertTrue(element=self.element,
				attr=self.kindAttr,
				condition=bool(resultValidate),
				message=str(resultValidate))

		# Resolve contract
		self.contracts.resolve(underlying.contracts)

		return underlying

	def getEntityResolved(self, symbols: "SymbolMap") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		return symbols.getEntity(fqn=self.kind).assertValue(element=self.element, attr=self.kindAttr)

	def getEntityUnderlyingTypeResolved(self, symbols: "SymbolMap") -> "EntityType":
		"""
		Get the entity related to type after resolve.
		"""
		entity = self.getEntityResolved(symbols=symbols)
		return entity.getEntityUnderlyingTypeResolved(symbols=symbols)

	@property
	def contracts(self) -> Contracts:
		return Contracts(element=self.element)

	@property
	def isTemplate(self) -> bool:
		return bool(self.templates)

	@cached_property
	def templates(self) -> "Parameters":
		return Parameters(element=self.element, nestedKind=self.templateAttr)

	@property
	def kind(self) -> str:
		return self.element.getAttr(self.kindAttr).value

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@cached_property
	def name(self) -> str:
		return Visitor(entity=self).result

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

		self.result = self.visitType(entity=entity, nested=template)

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

	def visitElement(self, element: Element, result: typing.List[str], nested: typing.List[str]) -> typing.List[str]:

		if element.isAttr("type"):

			entity = Type(element=element, kind="type", underlyingType="fqn_type", template="template")
			output = self.visitType(entity=entity, nested=nested)

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

	def visitType(self, entity: Type, nested: typing.List[str]) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		if nested:
			return "{}<{}>".format(entity.kind, ",".join(nested))
		return entity.kind

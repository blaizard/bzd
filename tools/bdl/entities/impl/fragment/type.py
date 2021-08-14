import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence, ElementBuilder
from bzd.parser.visitor import Visitor as VisitorBase
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
		ElementBuilder.cast(self.element, ElementBuilder).setAttr(self.kindAttr, fqn)

		# Resolve the templates if available
		self.templates.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		templates = self.templates

		# Get and save the underlying type
		# TODO: save is as part of the element itself so it is persisted.
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

			values = templates.getValuesOrTypesAsList(symbols=symbols, exclude=exclude)
			# Merge the lists
			resultValidate = validation.validate(values, output="return")
			Error.assertTrue(element=self.element,
				attr=self.kindAttr,
				condition=bool(resultValidate),
				message=str(resultValidate))

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


class Visitor(VisitorBase[str, str]):

	nestedKind = "template"

	def __init__(self, entity: Type) -> None:

		# Deal with the main type, do not include the comment as
		# it is taken care by the upper level block.
		kind = self.visitType(entity=entity, template=False)

		# Construct the template if any.
		if entity.templateAttr is not None:
			self.nestedKind = entity.templateAttr
			nested = entity.element.getNestedSequence(self.nestedKind)
			template = None if nested is None else self._visit(sequence=nested)
		else:
			template = None

		self.result = self.visitTypeTemplate(kind=kind, template=template)

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return self.visitTemplateItems(items=result)

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		if element.isAttr("type"):
			result.append(
				self.visitType(entity=Type(element=element, kind="type", underlyingType="fqn_type",
				template="template"),
				template=True))
		else:
			Error.assertHasAttr(element=element, attr="value")
			result.append(self.visitValue(value=element.getAttr("value").value,
				comment=element.getAttrValue("comment")))

		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.List[str]) -> typing.List[str]:
		nestedResult = self._visit(nestedSequence)
		result[-1] = self.visitTypeTemplate(kind=result[-1], template=nestedResult)
		return result

	def visitType(self, entity: Type, template: bool) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		return entity.kind

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		return value

	def visitTemplateItems(self, items: typing.List[str]) -> str:
		"""
		Called once all template elements have been discovered.
		This function should assemble the elements together.
		"""

		return "<{}>".format(",".join(items))

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		"""
		Called to assemble a type with its template.
		"""

		if template is None:
			return kind
		return "{}{}".format(kind, template)

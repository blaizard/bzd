import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence
from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import EntityType
	from tools.bdl.visitors.preprocess.symbol_map import SymbolMap


class Type:

	def __init__(self, element: Element, kind: str, template: typing.Optional[str] = None) -> None:

		Error.assertHasAttr(element=element, attr=kind)
		self.element = element
		self.kindAttr = kind
		self.templateAttr = template

	def resolve(self,
		symbols: "SymbolMap",
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> "EntityType":
		"""
		Resolve the types and nested templates by updating their symbol to fqn.
		"""

		fqn = symbols.resolveFQN(name=self.kind, namespace=namespace, exclude=exclude)
		Error.assertTrue(element=self.element,
			attr=self.kindAttr,
			condition=(fqn is not None),
			message="Symbol '{}' in namespace '{}' could not be resolved.".format(self.kind, ".".join(namespace)))
		assert fqn is not None
		self.element.updateAttrValue(name=self.kindAttr, value=fqn)

		# Resolve the templates if available
		self.templates.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		templates = self.templates.getValuesOrTypesAsList(symbols=symbols, exclude=exclude)

		# Get and save the underlying type
		# TODO: save is as part of the element itself so it is persisted.
		underlying = symbols.getEntityAssert(fqn=fqn, element=self.element)

		# Validate template arguments
		validation = underlying.makeValidationForTemplate(symbols=symbols)
		if validation is None:
			Error.assertTrue(element=self.element,
				condition=(not bool(templates)),
				attr=self.kindAttr,
				message="Type '{}' does not support template arguments.".format(self.kind))
		else:
			result = validation.validate(templates, output="return")
			Error.assertTrue(element=self.element, attr=self.kindAttr, condition=bool(result), message=str(result))

		return underlying

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

	@cached_property
	def name(self) -> str:
		return Visitor(entity=self).result

	@property
	def isFQN(self) -> bool:
		"""
		Return True if this contains a fully qualified name already, False otherwise.
		"""
		return "." in self.kind

	def __repr__(self) -> str:
		return self.name


class Visitor(VisitorBase[str, str]):

	nestedKind = "template"

	def __init__(self, entity: Type) -> None:

		# Deal with the main type, do not include the comment as
		# it is taken care by the upper level block.
		kind = self.visitType(kind=entity.kind, comment=None)

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
			result.append(self.visitType(kind=element.getAttr("type").value, comment=element.getAttrValue("comment")))
		else:
			Error.assertHasAttr(element=element, attr="value")
			result.append(self.visitValue(value=element.getAttr("value").value,
				comment=element.getAttrValue("comment")))

		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.List[str]) -> typing.List[str]:
		nestedResult = self._visit(nestedSequence)
		result[-1] = self.visitTypeTemplate(kind=result[-1], template=nestedResult)
		return result

	def visitType(self, kind: str, comment: typing.Optional[str]) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		return kind

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

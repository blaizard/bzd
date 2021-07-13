import typing

from bzd.utils.memoized_property import memoized_property
from bzd.parser.element import Element, Sequence
from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.contract import Contracts
if typing.TYPE_CHECKING:
	from bdl.entities.all import EntityType


class Type:

	def __init__(self, element: Element, kind: str, template: typing.Optional[str] = None) -> None:

		Error.assertHasAttr(element=element, attr=kind)
		self.element = element
		self.kindAttr = kind
		self.templateAttr = template
		self.underlying = typing.Optional["EntityType"]

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> "EntityType":
		"""
		Resolve the types and nested templates by updating their symbol to fqn.
		"""

		fqn = symbols.resolveFQN(name=self.kind, namespace=namespace, exclude=exclude)
		Error.assertTrue(element=self.element,
			condition=(fqn is not None),
			message="Symbol '{}' in namespace '{}' could not be resolved.".format(self.kind, ".".join(namespace)))
		self.element.updateAttrValue(name=self.kindAttr, value=fqn)

		# Loop through the nested templates.
		templates = {}
		for i, subType in enumerate(self.templates):
			# Recursively resolve nested types.
			subType.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
			# Only keep the kind, as nested template arguments will be deducted by recursion.
			templates[str(i)] = subType.kind

		# Get and save the underlying type
		self.underlying = symbols.getEntity(fqn=fqn)

		# Validate template arguments
		validation = self.underlying.validationTemplate # type: ignore
		if validation is None:
			Error.assertTrue(element=self.element,
				condition=(not bool(templates)),
				message="Type '{}' does not support template arguments.".format(self.kind))
		else:
			Error.assertTrue(element=self.element,
				condition=bool(templates),
				message="Type '{}' requires template arguments.".format(self.kind))
			result = validation.validate(templates, output="return")
			Error.assertTrue(element=self.element, condition=result, message=str(result))

		return self.underlying

	@property
	def contracts(self) -> Contracts:
		#if self.underlying is None:
		return Contracts(sequence=self.element.getNestedSequence("contract"))
		#return self.resolved_contracts

	@memoized_property
	def resolved_contracts(self) -> Contracts:
		assert self.underlying is not None, "The 'resolve' method must be callied prior to calling 'resolved_contracts'."
		# TODO merge contracts
		return self.contracts

	@property
	def isTemplate(self) -> bool:
		return len(self.templates) > 0

	@memoized_property
	def templates(self) -> typing.List["Type"]:
		if self.templateAttr:
			nested = self.element.getNestedSequence(self.templateAttr)
			if nested:
				# Note the "kind" here is always type for sub-elements.
				return [Type(element=element, kind="type", template=self.templateAttr) for element in nested]
		return []

	@property
	def kind(self) -> str:
		return self.element.getAttr(self.kindAttr).value

	@memoized_property
	def name(self) -> str:
		return Visitor(entity=self).result

	@property
	def isFQN(self) -> bool:
		"""
		Return True if this contains a fully qualified name already, False otherwise.
		"""
		return "." in self.kind

	def __repr__(self) -> str:
		return self.name  # type: ignore


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
		Error.assertHasAttr(element=element, attr="type")
		result.append(self.visitType(kind=element.getAttr("type").value, comment=element.getAttrValue("comment")))
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

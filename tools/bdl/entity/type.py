import typing

from bzd.parser.element import Element, Sequence
from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error

from tools.bdl.entity.contract import Contracts


class Type:

	def __init__(self, element: Element) -> None:

		Error.assertHasAttr(element=element, attr="type")
		self.element = element
		self.underlying: typing.Optional[typing.Any] = None

	def setUnderlying(self, underlying: typing.Any) -> None:
		"""
		Set the underlying type
		"""
		self.underlying = underlying

	@property
	def kind(self) -> str:
		return self.element.getAttr("type").value

	@property
	def name(self) -> str:
		return Visitor(entity=self).result

	@property
	def hasUnderlying(self) -> bool:
		return (self.underlying is not None)


class Visitor(VisitorBase[str, str]):

	nestedKind = "template"

	def __init__(self, entity: Type) -> None:

		# Deal with the main type, do not include the comment as
		# it is taken care by the upper level block.
		kind = self.visitType(kind=entity.kind, comment=None)

		# Construct the template if any.
		nested = entity.element.getNestedSequence("template")
		template = None if nested is None else self._visit(sequence=nested)

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

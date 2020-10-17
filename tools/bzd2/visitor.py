import typing

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import handleFromElement, assertHasAttr, assertHasSequence
from bzd.parser.element import Sequence, Element


class VisitorType(VisitorBase):

	nestedKind = "template"

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return self.visitTemplateItems(items=result)

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		assertHasAttr(element=element, attr="type")
		result.append(self.visitType(kind=element.getAttrValue("type"), comment=element.getAttrValue("comment")))
		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.List[str]) -> typing.List[str]:
		nestedResult = self._visit(nestedSequence, [])
		result[-1] = self.visitTypeTemplate(kind=result[-1], template=nestedResult)
		return result

	def visit(self, element: Element) -> str:

		# Deal with the main type, do not include the comment as
		# it is taken care by the main visitComment block.
		assertHasAttr(element=element, attr="type")
		kind = self.visitType(kind=element.getAttrValue("type"),comment=None)

		# Construct the template if any.
		template = None
		if element.isNestedSequence("template"):
			sequence = element.getNestedSequence("template")
			assert sequence is not None
			template = self._visit(sequence=sequence)

		return self.visitTypeTemplate(kind=kind, template=template)

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

		return ""

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]):
		"""
		Called to assemble a type with its template.
		"""

		return ""


class VisitorContract(VisitorBase):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return self.visitContractItems(items=result)

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		assertHasAttr(element=element, attr="type")
		result.append(
			self.visitContract(kind=element.getAttrValue("type"),
			value=element.getAttrValue("value"),
			comment=element.getAttrValue("comment")))
		return result

	def visitContractItems(self, items: typing.List[str]) -> str:
		"""
		Called once all contract elements have been discovered.
		This function should assemble the elements together.
		"""

		return ""

	def visitContract(self, kind: str, value: typing.Optional[str], comment: typing.Optional[str]) -> str:
		"""
		Called when a single contract needs to be formatted.
		"""

		return kind


class Visitor(VisitorBase):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0

	def visitBegin(self, result: typing.Any) -> str:
		return ""

	def makeIndent(self, level: int) -> str:
		return "\t" * level

	def applyIndent(self, result: str) -> str:
		indent = self.makeIndent(self.level)
		return "\n".join(["{}{}".format(indent, line) if len(line.strip()) else line for line in result.split("\n")])

	def visitElement(self, element: Element, result: str) -> str:

		assertHasAttr(element=element, attr="category")

		# Handle comments
		comment = element.getAttrValue("comment", "")
		if comment:
			result += self.applyIndent(self.visitComment(comment=comment))

		# Handle class
		if element.getAttrValue("category") == "class":

			assertHasAttr(element=element, attr="type")
			assertHasAttr(element=element, attr="name")
			assertHasSequence(element=element, sequence="nested")

			result += self.applyIndent(self.visitClassBegin(element=element))
			self.level += 1

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			result += self.visit(sequence)

			self.level -= 1
			result += self.applyIndent(self.visitClassEnd(element=element))

		# Handle variable
		elif element.getAttrValue("category") == "variable":

			assertHasAttr(element=element, attr="type")
			assertHasAttr(element=element, attr="name")

			result += self.applyIndent(self.visitVariable(element=element))

		# Should never go here
		else:
			raise Exception("Unexpected element category: {}", element.getAttrValue("category"))

		return result

	def visitVariable(self, element: Element) -> str:
		"""
		Called when discovering a variable.
		"""

		return ""

	def visitClassBegin(self, element: Element) -> str:
		"""
		Called when discovering a new class.
		"""

		return ""

	def visitClassEnd(self, element: Element) -> str:
		"""
		Called when ending the current class.
		"""

		return ""

	def visitComment(self, comment: str) -> str:
		"""
		Called for each comments.
		"""

		return ""

import typing

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import handleFromElement, assertHasAttr, assertHasSequence
from bzd.parser.element import Sequence, Element


class VisitorType(VisitorBase[str, str]):

	nestedKind = "template"

	def __init__(self, element: Element) -> None:

		# Deal with the main type, do not include the comment as
		# it is taken care by the main visitComment block.
		assertHasAttr(element=element, attr="type")
		kind = self.visitType(kind=element.getAttr("type").value, comment=None)

		# Construct the template if any.
		template = None
		if element.isNestedSequence("template"):
			sequence = element.getNestedSequence("template")
			assert sequence is not None
			template = self._visit(sequence=sequence)

		self.result = self.visitTypeTemplate(kind=kind, template=template)

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return self.visitTemplateItems(items=result)

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		assertHasAttr(element=element, attr="type")
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

		return ""

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		"""
		Called to assemble a type with its template.
		"""

		return ""


class VisitorContract(VisitorBase[str, str]):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return self.visitContractItems(items=result)

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		assertHasAttr(element=element, attr="type")
		result.append(
			self.visitContract(kind=element.getAttr("type").value,
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


T = typing.TypeVar("T")


class Visitor(VisitorBase[T, str]):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0

	def applyIndent(self, result: str) -> str:
		"""
		Helper function to add indentation to blocks.
		"""

		indent = "\t" * self.level
		return "\n".join(["{}{}".format(indent, line) if len(line.strip()) else line for line in result.split("\n")])

	def visitElement(self, element: Element, result: T) -> T:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		assertHasAttr(element=element, attr="category")

		# Handle class
		if element.getAttr("category").value == "class":

			assertHasAttr(element=element, attr="type")
			assertHasAttr(element=element, attr="name")
			assertHasSequence(element=element, sequence="nested")

			self.level += 1

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			nestedResult = self._visit(sequence)

			self.level -= 1

			result = self.visitClass(result=result, nestedResult=nestedResult, element=element)

		# Handle variable
		elif element.getAttr("category").value == "variable":

			assertHasAttr(element=element, attr="type")
			assertHasAttr(element=element, attr="name")

			result = self.visitVariable(result=result, element=element)

		# Should never go here
		else:
			raise Exception("Unexpected element category: {}", element.getAttr("category").value)

		return result

	def visitFinal(self, result: T) -> str:
		return str(result)

	def visitClass(self, result: T, nestedResult: T, element: Element) -> T:
		"""
		Called when discovering a class.
		"""

		return result

	def visitVariable(self, result: T, element: Element) -> T:
		"""
		Called when discovering a variable.
		"""

		return result

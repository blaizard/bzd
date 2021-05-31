import typing
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import handleFromElement, assertHasAttr, assertHasSequence
from bzd.parser.element import Sequence, Element

from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use

T = typing.TypeVar("T")


class Visitor(VisitorBase[T, str]):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0
		self.hasNamespace = False
		self.type= None

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
		if element.getAttr("category").value == "nested":

			assertHasSequence(element=element, sequence="nested")

			self.level += 1

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			nestedResult = self._visit(sequence)

			self.level -= 1

			result = self.visitNested(result=result,
				nestedResult=nestedResult,
				element=element,
				entity=Nested(element=element, nested=nestedResult, visitor=self))

		# Handle variable
		elif element.getAttr("category").value == "variable":

			result = self.visitVariable(result=result, element=element, entity=Variable(element=element, visitor=self))

		# Handle method
		elif element.getAttr("category").value == "method":

			result = self.visitMethod(result=result, element=element, entity=Method(element=element, visitor=self))

		# Handle using
		elif element.getAttr("category").value == "using":

			result = self.visitUsing(result=result, element=element, entity=Using(element=element, visitor=self))

		# Handle namespace
		elif element.getAttr("category").value == "namespace":

			if self.hasNamespace:
				handleFromElement(element=element, message="Only a single namespace can be specified per file.")
			self.hasNamespace = True

			result = self.visitNamespace(result=result, element=element, entity=Namespace(element=element))

		# Handle use
		elif element.getAttr("category").value == "use":

			result = self.visitUse(result=result, entity=Use(element=element))

		# Should never go here
		else:
			raise Exception("Unexpected element category: {}", element.getAttr("category").value)

		return result

	def visitFinal(self, result: T) -> str:
		return str(result)

	def visitComment(self, context: typing.Any, comment: str) -> str:
		"""
		Called when a comment is discovered.
		"""

		return comment

	def visitNested(self, result: T, nestedResult: T, element: Element, entity: Nested) -> T:
		"""
		Called when discovering a nested entity.
		"""

		return result

	def visitVariable(self, result: T, element: Element, variable: Variable) -> T:
		"""
		Called when discovering a variable.
		"""

		return result

	def visitMethod(self, result: T, element: Element, entity: Method) -> T:
		"""
		Called when discovering a method.
		"""

		return result

	def visitUsing(self, result: T, element: Element, entity: Using) -> T:
		"""
		Called when discovering a using keyword.
		"""

		return result

	def visitNamespace(self, result: T, element: Element, entity: Namespace) -> T:
		"""
		Called when discovering a namespace.
		"""

		return result

	def visitUse(self, result: T, entity: Use) -> T:
		"""
		Called when discovering an use statement.
		"""

		return result

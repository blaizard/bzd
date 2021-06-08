import typing
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error
from bzd.parser.element import Element

from tools.bdl.result import ResultType

from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.enum import Enum
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use


class Visitor(VisitorBase[ResultType, str]):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0

	def visitBegin(self, result: typing.Any) -> ResultType:
		return ResultType(level=self.level)

	def visitElement(self, element: Element, result: ResultType) -> ResultType:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		Error.assertHasAttr(element=element, attr="category")

		# Handle class
		if element.getAttr("category").value == "nested":

			Error.assertHasSequence(element=element, sequence="nested")

			self.level += 1

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			nestedResult = self._visit(sequence)

			self.level -= 1

			nested = self.visitNestedEntities(
				entity=Nested(element=element, nested=typing.cast(typing.List[typing.Any], nestedResult)))
			result.registerSymbol(element=element, entity=nested)

		# Handle variable
		elif element.getAttr("category").value == "variable":

			variable = self.visitVariable(entity=Variable(element=element))
			result.registerSymbol(element=element, entity=variable)

		# Handle method
		elif element.getAttr("category").value == "method":

			method = self.visitMethod(entity=Method(element=element))
			result.registerSymbol(element=element, entity=method)

		# Handle using
		elif element.getAttr("category").value == "using":

			using = self.visitUsing(entity=Using(element=element))
			result.registerSymbol(element=element, entity=using)

		# Handle using
		elif element.getAttr("category").value == "enum":

			enum = self.visitEnum(entity=Enum(element=element))
			result.registerSymbol(element=element, entity=enum)

		# Handle namespace
		elif element.getAttr("category").value == "namespace":

			namespace = self.visitNamespace(entity=Namespace(element=element))
			result.registerNamespace(element=element, entity=namespace)

		# Handle use
		elif element.getAttr("category").value == "use":

			use = self.visitUse(entity=Use(element=element))
			result.registerUse(element=element, entity=use)

		# Should never go here
		else:
			raise Exception("Unexpected element category: {}", element.getAttr("category").value)

		return result

	def visitFinal(self, result: ResultType) -> str:
		return str(result)

	def visitNestedEntities(self, entity: Nested) -> Nested:
		"""
		Called when discovering a nested entity.
		"""

		return entity

	def visitVariable(self, entity: Variable) -> Variable:
		"""
		Called when discovering a variable.
		"""

		return entity

	def visitMethod(self, entity: Method) -> Method:
		"""
		Called when discovering a method.
		"""

		return entity

	def visitUsing(self, entity: Using) -> Using:
		"""
		Called when discovering a using keyword.
		"""

		return entity

	def visitEnum(self, entity: Enum) -> Enum:
		"""
		Called when discovering an enum.
		"""

		return entity

	def visitNamespace(self, entity: Namespace) -> Namespace:
		"""
		Called when discovering a namespace.
		"""

		return entity

	def visitUse(self, entity: Use) -> Use:
		"""
		Called when discovering an use statement.
		"""

		return entity

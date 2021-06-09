import typing
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error
from bzd.parser.element import Element

from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.enum import Enum
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use

T = typing.TypeVar("T")
U = typing.TypeVar("U")


class Visitor(VisitorBase[T, U]):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0
		self.namespace: typing.List[str] = []

	def visitElement(self, element: Element, result: T) -> T:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		Error.assertHasAttr(element=element, attr="category")

		# Handle nested object
		if element.getAttr("category").value == "nested":

			Error.assertHasSequence(element=element, sequence="nested")
			nested = Nested(element=element)

			self.level += 1
			self.namespace.append(nested.name)

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			nestedResult = self._visit(sequence)

			self.namespace.pop()
			self.level -= 1

			nested.setNested(nested=typing.cast(typing.List[typing.Any], nestedResult))
			self.visitNestedEntities(entity=nested, result=result)

		# Handle variable
		elif element.getAttr("category").value == "variable":

			self.visitVariable(entity=Variable(element=element), result=result)

		# Handle method
		elif element.getAttr("category").value == "method":

			self.visitMethod(entity=Method(element=element), result=result)

		# Handle using
		elif element.getAttr("category").value == "using":

			self.visitUsing(entity=Using(element=element), result=result)

		# Handle using
		elif element.getAttr("category").value == "enum":

			self.visitEnum(entity=Enum(element=element), result=result)

		# Handle namespace
		elif element.getAttr("category").value == "namespace":

			Error.assertTrue(element=element,
				condition=(self.level == 0),
				message="Namespaces can only be declared at top level.")

			namespace = Namespace(element=element)
			self.visitNamespace(entity=namespace, result=result)

			# Update the current namespace
			self.namespace.extend(namespace.nameList)

		# Handle use
		elif element.getAttr("category").value == "use":

			self.visitUse(entity=Use(element=element), result=result)

		# Should never go here
		else:
			Error.handleFromElement(element=element,
				message="Unexpected element category: {}".format(element.getAttr("category").value))

		return result

	def visitNestedEntities(self, entity: Nested, result: T) -> None:
		"""
		Called when discovering a nested entity.
		"""
		pass

	def visitVariable(self, entity: Variable, result: T) -> None:
		"""
		Called when discovering a variable.
		"""
		pass

	def visitMethod(self, entity: Method, result: T) -> None:
		"""
		Called when discovering a method.
		"""
		pass

	def visitUsing(self, entity: Using, result: T) -> None:
		"""
		Called when discovering a using keyword.
		"""
		pass

	def visitEnum(self, entity: Enum, result: T) -> None:
		"""
		Called when discovering an enum.
		"""
		pass

	def visitNamespace(self, entity: Namespace, result: T) -> None:
		"""
		Called when discovering a namespace.
		"""
		pass

	def visitUse(self, entity: Use, result: T) -> None:
		"""
		Called when discovering an use statement.
		"""
		pass

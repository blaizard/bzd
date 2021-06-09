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

NamespaceType = typing.List[str]
EntityType = typing.Union[Variable, Nested, Method, Using, Enum, Namespace, Use]

T = typing.TypeVar("T")


class Visitor(VisitorBase[T, T]):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0
		self.namespace: NamespaceType = []

	def visitElement(self, element: Element, result: T) -> T:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		entity = self.elementToEntity(element=element)

		# Handle nested object
		if isinstance(entity, Nested):

			Error.assertHasSequence(element=element, sequence="nested")

			self.level += 1
			self.namespace.append(entity.name)

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			nestedResult = self._visit(sequence)

			self.namespace.pop()
			self.level -= 1

			entity.setNested(nested=typing.cast(typing.List[typing.Any], nestedResult))
			self.visitNestedEntities(entity=entity, result=result)

		# Handle variable
		elif isinstance(entity, Variable):

			self.visitVariable(entity=entity, result=result)

		# Handle method
		elif isinstance(entity, Method):

			self.visitMethod(entity=entity, result=result)

		# Handle using
		elif isinstance(entity, Using):

			self.visitUsing(entity=entity, result=result)

		# Handle using
		elif isinstance(entity, Enum):

			self.visitEnum(entity=entity, result=result)

		# Handle namespace
		elif isinstance(entity, Namespace):

			Error.assertTrue(element=element,
				condition=(self.level == 0),
				message="Namespaces can only be declared at top level.")

			self.visitNamespace(entity=entity, result=result)

			# Update the current namespace
			self.namespace.extend(entity.nameList)

		# Handle use
		elif isinstance(entity, Use):

			self.visitUse(entity=entity, result=result)

		# Should never go here
		else:
			raise Exception()

		return result

	def elementToEntity(self, element: Element) -> EntityType:
		"""
		Instantiate an entity from an element.
		"""

		categoryToEntity: typing.Dict[str, typing.Type[EntityType]] = {
			"nested": Nested,
			"variable": Variable,
			"method": Method,
			"using": Using,
			"enum": Enum,
			"namespace": Namespace,
			"use": Use
		}

		Error.assertHasAttr(element=element, attr="category")
		category = element.getAttr("category").value

		if category not in categoryToEntity:
			Error.handleFromElement(element=element, message="Unexpected element category: {}".format(category))

		return categoryToEntity[category](element=element)

	def makeFQN(self, name: str, namespace: typing.Optional[NamespaceType] = None) -> str:
		"""
		Make the fully qualified name from a symbol name
		"""
		return ".".join((self.namespace if namespace is None else namespace) + [name])

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

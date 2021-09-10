import typing
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error
from bzd.parser.element import Element, ElementSerialize
from bzd.parser.fragments import Attribute

from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use, EntityType, elementToEntity

NamespaceType = typing.List[str]

T = typing.TypeVar("T")

CATEGORY_COMPOSITION = "composition"
CATEGORY_CONFIG = "config"
CATEGORY_NESTED = "nested"
CATEGORY_GLOBAL = "global"
CATEGORY_GLOBAL_COMPOSITION = "globalComposition"
CATEGORIES = [CATEGORY_COMPOSITION, CATEGORY_CONFIG, CATEGORY_NESTED, CATEGORY_GLOBAL, CATEGORY_GLOBAL_COMPOSITION]


class Parent:

	def __init__(self, entity: typing.Union[Nested, Namespace], category: typing.Optional[str] = None) -> None:
		self.entity = entity
		self.category = category

	@property
	def isNested(self) -> bool:
		return isinstance(self.entity, Nested)

	@property
	def namespace(self) -> typing.List[str]:
		if self.isNested:
			return [self.entity.name] if self.entity.isName else []
		if isinstance(self.entity, Namespace):
			return self.entity.nameList
		return []


class Visitor(VisitorBase[T, T]):

	nestedKind = None

	def __init__(self,
		elementToEntityExtenstion: typing.Optional[typing.Dict[str, typing.Type[EntityType]]] = None) -> None:
		self.level = 0
		self.parents: typing.List[Parent] = []
		self.elementToEntityExtenstion = elementToEntityExtenstion

	@property
	def parent(self) -> typing.Optional[Parent]:
		return self.parents[-1] if len(self.parents) else None

	@property
	def namespace(self) -> typing.List[str]:
		"""
		Get the list of name constituing the namespace.
		"""
		return [name for parent in self.parents for name in parent.namespace]

	@property
	def category(self) -> str:
		"""
		Return the current category: global, composition, config
		"""
		for parent in reversed(self.parents):
			if parent.category in [CATEGORY_COMPOSITION, CATEGORY_CONFIG]:
				return parent.category
			if parent.isNested:
				assert isinstance(parent.entity, Nested)
				if parent.entity.type == "composition":
					return CATEGORY_GLOBAL_COMPOSITION
		return CATEGORY_GLOBAL

	def visitElement(self, element: Element, result: T) -> T:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		entity = elementToEntity(element=element, extension=self.elementToEntityExtenstion)

		# Handle nested object
		if isinstance(entity, Nested):

			Error.assertHasSequence(element=element, sequence=CATEGORY_NESTED)

			self.level += 1

			for category in CATEGORIES:
				sequence = element.getNestedSequence(category)
				if sequence is not None:
					self.parents.append(Parent(entity=entity, category=category))
					nestedResult = self._visit(sequence, result)
					self.entityToNested(category, entity, nestedResult, result)
					self.parents.pop()

			self.level -= 1

			self.visitNestedEntities(entity, result)

		# Handle expression
		elif isinstance(entity, Expression):

			self.visitExpression(entity, result)

		# Handle method
		elif isinstance(entity, Method):

			self.visitMethod(entity, result)

		# Handle using
		elif isinstance(entity, Using):

			self.visitUsing(entity, result)

		# Handle using
		elif isinstance(entity, Enum):

			self.visitEnum(entity, result)

		# Handle namespace
		elif isinstance(entity, Namespace):

			Error.assertTrue(element=element,
				condition=(self.level == 0),
				message="Namespaces can only be declared at top level.")

			self.visitNamespace(entity, result)

			# Update the current namespace
			self.parents.append(Parent(entity=entity))

		# Handle use
		elif isinstance(entity, Use):

			self.visitUse(entity, result)

		# Should never go here
		else:
			Error.handleFromElement(element=element, message="Unexpected entity: {}".format(type(entity)))

		return result

	def entityToNested(self, category: str, entity: Nested, nested: T, result: T) -> None:
		"""
		Associate a nested result with its entity.
		"""
		pass

	def visitNestedEntities(self, entity: Nested, result: T) -> None:
		"""
		Called when discovering a nested entity.
		"""
		pass

	def visitExpression(self, entity: Expression, result: T) -> None:
		"""
		Called when discovering an expression.
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

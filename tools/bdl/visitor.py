import typing
import enum
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error
from bzd.parser.element import Element, ElementSerialize
from bzd.parser.fragments import Attribute

from tools.bdl.entities.impl.types import Category
from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Extern, Namespace, Use, EntityType, elementToEntity

NamespaceType = typing.List[str]


class NestedSequence(enum.Enum):
	composition = "composition"
	config = "config"
	interface = "interface"


class Group(enum.Enum):
	composition = "composition"
	config = "config"
	interface = "interface"
	globalGroup = "globalGroup"
	globalComposition = "globalComposition"


class Parent:

	def __init__(self, entity: typing.Optional[EntityType] = None, group: typing.Optional[Group] = None) -> None:
		self.entity = entity
		self.group = group

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


T = typing.TypeVar("T")


class Visitor(VisitorBase[T, T]):

	nestedKind = None

	def __init__(self,
		elementToEntityExtenstion: typing.Optional[typing.Dict[str, typing.Type[EntityType]]] = None) -> None:
		self.level = 0
		self.parents: typing.List[Parent] = []
		self.elementToEntityExtenstion = elementToEntityExtenstion

	@property
	def namespace(self) -> typing.List[str]:
		"""
		Get the list of name constituing the namespace.
		"""
		return [name for parent in self.parents for name in parent.namespace]

	@property
	def group(self) -> Group:
		"""
		Return the current group: global, composition, config
		"""
		for parent in reversed(self.parents):
			#if parent.group is not None:
			if parent.group is not None:
				if parent.group in {Group.globalComposition, Group.composition, Group.config}:
					return parent.group
		return Group.globalGroup

	def entityToGroup(self, entity: EntityType) -> typing.Optional[Group]:
		"""Define a group based on an entity."""

		if entity.category == Category.composition:
			return Group.globalComposition
		elif entity.category == Category.interface:
			return Group.interface
		return None

	def nestedToGroup(self, entity: EntityType, nested: NestedSequence) -> typing.Optional[Group]:
		"""Convert a nested sequence type into a group."""

		return {
			NestedSequence.composition: Group.composition,
			NestedSequence.interface: Group.interface,
			NestedSequence.config: Group.config,
		}[nested]

	def visitElement(self, element: Element, result: T) -> T:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		entity = elementToEntity(element=element, extension=self.elementToEntityExtenstion)
		self.parents.append(Parent(group=self.entityToGroup(entity=entity)))

		# Handle nested object
		if isinstance(entity, Nested):

			self.level += 1

			for nested in NestedSequence:
				sequence = element.getNestedSequence(nested.value)
				if sequence is not None:
					self.parents.append(Parent(entity=entity, group=self.nestedToGroup(entity, nested)))
					nestedResult = self._visit(sequence, result)
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

		# Handle extern
		elif isinstance(entity, Extern):

			self.visitExtern(entity, result)

		# Handle enum
		elif isinstance(entity, Enum):

			self.visitEnum(entity, result)

		# Handle namespace
		elif isinstance(entity, Namespace):

			Error.assertTrue(element=element,
				condition=(self.level == 0),
				message="Namespaces can only be declared at top level.")

			self.visitNamespace(entity, result)

			# Update the current namespace. This is not a popable element, hence insert at the begining.
			self.parents.insert(0, Parent(entity=entity))

		# Handle use
		elif isinstance(entity, Use):

			self.visitUse(entity, result)

		# Should never go here
		else:
			Error.handleFromElement(element=element, message="Unexpected entity: {}".format(type(entity)))

		self.parents.pop()

		return result

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

	def visitExtern(self, entity: Extern, result: T) -> None:
		"""
		Called when discovering an extern keyword.
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

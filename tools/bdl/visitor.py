import typing
import enum
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error
from bzd.parser.element import Element, ElementSerialize
from bzd.parser.fragments import Attribute

from tools.bdl.entities.impl.types import Category
from tools.bdl.entities.all import (
    Expression,
    Builtin,
    Nested,
    Method,
    Using,
    Enum,
    Extern,
    Namespace,
    Use,
    EntityType,
    elementToEntity,
)

NamespaceType = typing.List[str]


class NestedSequence(enum.Enum):
	composition = "composition"
	config = "config"
	interface = "interface"


class Parent:

	def __init__(self, entity: typing.Optional[typing.Union[EntityType, typing.List[str]]] = None) -> None:
		self.entity = entity

	@property
	def isNested(self) -> bool:
		return isinstance(self.entity, Nested)

	@property
	def namespace(self) -> typing.List[str]:
		if self.entity and self.isNested:
			return [self.entity.name] if self.entity.isName else []  # type: ignore
		if isinstance(self.entity, Namespace):
			return self.entity.nameList
		if isinstance(self.entity, list):
			return self.entity
		return []


class Group(enum.Flag):
	builtin = enum.auto()
	component = enum.auto()
	composition = enum.auto()
	interface = enum.auto()
	struct = enum.auto()
	config = enum.auto()
	topLevel = enum.auto()
	nested = enum.auto()


class GroupScope:

	def __init__(self, list: typing.List[Group], group: typing.Optional[Group]) -> None:
		self.list = list
		self.group = group

	def __enter__(self) -> None:
		if self.group is not None:
			self.list.append(self.group)

	def __exit__(self, exc_type: typing.Any, exc_value: typing.Any, exc_tb: typing.Any) -> None:
		if self.group is not None:
			self.list.pop()


class Groups:

	def __init__(self) -> None:
		self.list: typing.List[Group] = []

	def scope(self, group: typing.Optional[Group]) -> GroupScope:
		return GroupScope(list=self.list, group=group)

	@property
	def group(self) -> Group:
		"""Return the current group: global, composition, config."""

		if len(self.list) == 0:
			return Group.topLevel

		if all(group == self.list[0] for group in self.list):
			return self.list[0] | Group.topLevel

		return self.list[-1] | Group.nested


T = typing.TypeVar("T")


class Visitor(VisitorBase[T, T]):
	nestedKind = None

	def __init__(
	    self,
	    namespace: typing.Optional[typing.List[str]] = None,
	    elementToEntityExtenstion: typing.Optional[typing.Dict[str, typing.Type[EntityType]]] = None,
	) -> None:
		"""Create a visitor for the bdl grammar.

        Args:
                namespace: An optional namespace to start the discovery from.
                elementToEntityExtenstion: Extension to be used when a new element is discovered.
        """

		self.level = 0
		self.parents: typing.List[Parent] = []
		self.elementToEntityExtenstion = elementToEntityExtenstion
		self.groups = Groups()

		# Insert the initial namespace if any.
		if namespace:
			self.parents.append(Parent(entity=namespace))

	@property
	def namespace(self) -> typing.List[str]:
		"""
        Get the list of name constituing the namespace.
        """
		return [name for parent in self.parents for name in parent.namespace]

	@property
	def group(self) -> Group:
		return self.groups.group

	def entityToGroup(self, entity: EntityType) -> typing.Optional[Group]:
		"""Define a group based on an entity."""

		if entity.category == Category.composition:
			return Group.composition
		elif entity.category == Category.interface:
			return Group.interface
		elif entity.category == Category.component:
			return Group.component
		elif entity.category == Category.struct:
			return Group.struct
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
		with self.groups.scope(group=self.entityToGroup(entity=entity)):
			# Handle nested object
			if isinstance(entity, Nested):
				self.level += 1

				entity.assertTrue(
				    condition=not element.isNestedSequence("invalid")
				    or element.getNestedSequence("invalid").empty(),  # type: ignore
				    message=f"This element '{entity.category}' does not support direct nested scope.",
				)
				for nested in NestedSequence:
					sequence = element.getNestedSequence(nested.value)
					if sequence is not None:
						self.parents.append(Parent(entity=entity))
						with self.groups.scope(group=self.nestedToGroup(entity, nested)):
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
				Error.assertTrue(
				    element=element,
				    condition=(self.level == 0),
				    message="Namespaces can only be declared at top level.",
				)

				self.visitNamespace(entity, result)

				# Update the current namespace. This is not a popable element, hence insert at the begining.
				self.parents.append(Parent(entity=entity))

			# Handle use
			elif isinstance(entity, Use):
				self.visitUse(entity, result)

			# Should never go here
			else:
				Error.handleFromElement(
				    element=element,
				    message="Unexpected entity: {}".format(type(entity)),
				)

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

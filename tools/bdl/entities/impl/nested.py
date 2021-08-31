import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.fragment.fqn import FQN

TYPE_STRUCT = "struct"
TYPE_COMPONENT = "component"
TYPE_INTERFACE = "interface"
TYPE_COMPOSITION = "composition"
TYPES = [TYPE_STRUCT, TYPE_COMPONENT, TYPE_INTERFACE, TYPE_COMPOSITION]


class Nested(Entity):
	"""
	Defines a nested entity such as a struct, a component or an interface.
	Nested entities have the following underlying elements:
	- Attributes:
		- type: struct, component, interface...
		- [name]: The name of this entity, for example `struct MyType` would have the name MyType.
	- Sequences:
		- [inheritance]: In case the the struct have one or multiple base class.
		- [config]
		- [nested]
		- [composition]
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="type")
		self.assertTrue(condition=self.type in TYPES, message="Unsupported nested type: '{}'.".format(self.type))

	@property
	def configAttr(self) -> str:
		return "nested" if self.type == "struct" else "config"

	@property
	def category(self) -> str:
		return "nested"

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	@property
	def hasInheritance(self) -> bool:
		return self.element.isNestedSequence("inheritance")

	@cached_property
	def inheritanceList(self) -> typing.List[Type]:
		sequence = self.element.getNestedSequence("inheritance")
		if sequence is None:
			return []
		inheritanceList: typing.List[Type] = []
		for element in sequence:
			Error.assertHasAttr(element=element, attr="symbol")
			inheritanceList.append(Type(element=element, kind="symbol"))
		return inheritanceList

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve entities.
		"""
		# Generate this symbol FQN.
		if self.isName:
			fqn = FQN.fromNamespace(name=self.name, namespace=namespace)
			self._setUnderlyingType(fqn)

		# Resolve and make sure the inheritance is correct.
		for inheritance in self.inheritanceList:

			# Resolve the inheritance.
			entity = inheritance.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
			self.addParents(fqn=inheritance.kind, parents=entity.getParents())

			# Validates that the inheritance type is correct.
			underlyingType = entity.getEntityUnderlyingTypeResolved(symbols=symbols)
			self.assertTrue(condition=underlyingType.category == "nested",
				message="Inheritance can only be done from a nested class, not '{}'.".format(entity.underlyingType))
			nestedType = typing.cast("Nested", underlyingType)
			if self.type == TYPE_STRUCT:
				self.assertTrue(condition=nestedType.type == TYPE_STRUCT,
					message="A struct can only inherits from another struct, not '{}'.".format(nestedType.type))
			elif self.type == TYPE_INTERFACE:
				self.assertTrue(condition=nestedType.type == TYPE_INTERFACE,
					message="An interface can only inherits from another interface, not '{}'.".format(nestedType.type))
			elif self.type == TYPE_COMPONENT:
				self.assertTrue(condition=nestedType.type == TYPE_INTERFACE,
					message="A component can only inherits from interface(s), not '{}'.".format(nestedType.type))
			else:
				self.error(message="Unsupported inheritance for type: '{}'.".format(self.type))

	def __repr__(self) -> str:
		content = self.toString({
			"name": self.name if self.isName else "",
			"type": self.type,
			"inheritance": ", ".join([str(t) for t in self.inheritanceList])
		})

		for category in ["config", "nested", "composition"]:
			nested = self._getNestedByCategory(category)
			if nested:
				content += "\n{}:".format(category)
				content += "\n  ".join([""] + "\n".join([str(entity) for entity in nested]).split("\n"))

		return content

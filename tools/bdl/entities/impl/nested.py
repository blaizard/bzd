import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor

from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.entity import Entity, Role
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.types import Category


class Nested(Entity):
	"""
	Defines a nested entity such as a struct, a component or an interface.
	Nested entities have the following underlying elements:
	- Attributes:
		- category: struct, component, interface... see Nested.categoriesAllowed_
		- [name]: The name of this entity, for example `struct MyType` would have the name MyType.
	- Sequences:
		- [inheritance]: In case the the struct have one or multiple base class.
		- [config]
		- [interface]
		- [composition]
	"""

	categoriesAllowed_ = [Category.struct, Category.component, Category.interface, Category.composition]

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Type)
		self.assertTrue(condition=self.category in Nested.categoriesAllowed_,
			message=
			f"Unsupported nested type: '{self.category}', only the following are supported: {Nested.categoriesAllowed_}"
						)

	@property
	def configAttr(self) -> str:
		# TODO: handle inheritance for "struct"
		return "interface" if self.category == Category.struct else "config"

	@property
	def hasInheritance(self) -> bool:
		return self.element.isNestedSequence("inheritance")

	@cached_property
	def inheritanceList(self) -> typing.List[Symbol]:
		inheritanceList: typing.List[Symbol] = []
		for element in self.element.getNestedSequenceOrEmpty("inheritance"):
			Error.assertHasAttr(element=element, attr="symbol")
			inheritanceList.append(Symbol(element=element, kind="symbol"))
		return inheritanceList

	def resolve(self, resolver: typing.Any) -> None:
		"""
		Resolve entities.
		"""
		# Generate this symbol FQN.
		if self.isName:
			self._setUnderlyingTypeFQN(self.fqn)

		# Resolve and make sure the inheritance is correct.
		for inheritance in self.inheritanceList:

			# Resolve the inheritance.
			entity = inheritance.resolve(resolver=resolver)
			self.addParents(fqn=entity.underlyingTypeFQN, parents=entity.getUnderlyingTypeParents(resolver=resolver))

			# Validates that the inheritance type is correct.
			underlyingType = entity.getEntityUnderlyingTypeResolved(resolver=resolver)
			if underlyingType.category in Nested.categoriesAllowed_:
				nestedCategory = underlyingType.category  # type: ignore
			elif underlyingType.category == Category.extern:
				nestedCategory = underlyingType.type  # type: ignore
			else:
				self.error(message="Inheritance can only be done from a nested class, not '{}', category '{}'.".format(
					entity.underlyingTypeFQN, underlyingType.category))

			if self.category == Category.struct:
				self.assertTrue(condition=nestedCategory == Category.struct,
					message="A struct can only inherits from another struct, not '{}'.".format(nestedCategory))
			elif self.category == Category.interface:
				self.assertTrue(condition=nestedCategory == Category.interface,
					message="An interface can only inherits from another interface, not '{}'.".format(nestedCategory))
			elif self.category == Category.component:
				self.assertTrue(condition=nestedCategory == Category.interface,
					message="A component can only inherits from interface(s), not '{}'.".format(nestedCategory))
			else:
				self.error(message="Unsupported inheritance for type: '{}'.".format(self.category))

		super().resolve(resolver)

	def __repr__(self) -> str:
		content = self.toString({
			"name": self.name if self.isName else "",
			"category": self.category,
			"inheritance": ", ".join([str(t) for t in self.inheritanceList])
		})

		for category in ["config", "interface", "composition"]:
			nested = self._getNestedByCategory(category)
			if nested:
				content += "\n{}:".format(category)
				content += "\n  ".join([""] + "\n".join([str(entity) for entity in nested]).split("\n"))

		return content

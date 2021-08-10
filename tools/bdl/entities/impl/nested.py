import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.entity import Entity, Role


class _VisitorInheritance(Visitor[Type, typing.List[Type]]):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[Type]:
		return []

	def visitElement(self, element: Element, result: typing.List[Type]) -> typing.List[Type]:
		Error.assertHasAttr(element=element, attr="symbol")
		result.append(Type(element=element, kind="symbol"))
		return result


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

	@property
	def category(self) -> str:
		return "nested"

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@property
	def hasInheritance(self) -> bool:
		return self.element.isNestedSequence("inheritance")

	@cached_property
	def inheritanceList(self) -> typing.List[Type]:
		sequence = self.element.getNestedSequence("inheritance")
		if sequence is None:
			return []
		return _VisitorInheritance().visit(sequence=sequence)

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve entities.
		"""
		# Generate this symbol FQN
		if self.isName:
			fqn = symbols.namespaceToFQN(name=self.name, namespace=namespace)
			self._setUnderlyingType(fqn)

		for inheritance in self.inheritanceList:
			inheritance.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

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

import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.entity import Entity


class _VisitorInheritance(Visitor[Type, typing.List[Type]]):

	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[Type]:
		return []

	def visitElement(self, element: Element, result: typing.List[Type]) -> typing.List[Type]:
		Error.assertHasAttr(element=element, attr="symbol")
		result.append(Type(element=element, kind="symbol"))
		return result


class Nested(Entity):

	def __init__(self, element: Element) -> None:

		super().__init__(element)
		Error.assertHasAttr(element=element, attr="type")

	def _getNestedByCategory(self, category: str) -> typing.Any:
		sequence = self.element.getNestedSequence(category)
		if sequence:
			from tools.bdl.entities.all import elementToEntity
			return [elementToEntity(element) for element in sequence]
		return []

	@property
	def nested(self) -> typing.Any:
		return self._getNestedByCategory("nested")

	@property
	def config(self) -> typing.Any:
		return self._getNestedByCategory("config")

	@property
	def composition(self) -> typing.Any:
		return self._getNestedByCategory("composition")

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

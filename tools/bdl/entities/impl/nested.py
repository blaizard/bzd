import typing

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

		self.nestedCategories: typing.Dict[str, typing.Any] = {}

	def setNested(self, category: str, nested: typing.Any) -> None:
		self.nestedCategories[category] = nested

	@property
	def nested(self) -> typing.Any:
		return self.nestedCategories.get("nested", [])

	@property
	def isConfig(self) -> bool:
		return "config" in self.nestedCategories

	@property
	def config(self) -> typing.Any:
		return self.nestedCategories["config"]

	@property
	def isComposition(self) -> bool:
		return "composition" in self.nestedCategories

	@property
	def composition(self) -> typing.Any:
		return self.nestedCategories["composition"]

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

	@property
	def inheritanceList(self) -> typing.List[Type]:
		sequence = self.element.getNestedSequence("inheritance")
		if sequence is None:
			return []
		return _VisitorInheritance().visit(sequence=sequence)

	def resolve(self, symbols: typing.Any, namespace: typing.List[str]) -> None:
		"""
		Resolve entities.
		"""
		for inheritance in self.inheritanceList:
			inheritance.resolve(symbols=symbols, namespace=namespace)

	def __repr__(self) -> str:
		content = self.toString({
			"name": self.name if self.isName else "",
			"type": self.type,
			"inheritance": ", ".join([str(t) for t in self.inheritanceList])
		})
		for category, nested in self.nestedCategories.items():
			if nested:
				content += category + ":\n"
				content += "\n  ".join([""] + repr(nested).split("\n"))
		return content

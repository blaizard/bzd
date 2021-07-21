import typing

from bzd.parser.element import Element

from tools.bdl.entities.impl.nested import Nested


class NestedResult(Nested):

	def __init__(self, element: Element) -> None:

		super().__init__(element)
		self.nestedCategories: typing.Dict[str, typing.Any] = {}

	def setNested(self, category: str, nested: typing.Any) -> None:
		self.nestedCategories[category] = nested

	@property
	def isConfig(self) -> bool:
		return "config" in self.nestedCategories

	@property
	def config(self) -> typing.Any:
		return self.nestedCategories["config"]

	@property
	def nested(self) -> typing.Any:
		return self.nestedCategories.get("nested", [])

	@property
	def isComposition(self) -> bool:
		return "composition" in self.nestedCategories

	@property
	def composition(self) -> typing.Any:
		return self.nestedCategories["composition"]

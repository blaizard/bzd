from bzd.parser.element import ElementBuilder as Element, SequenceBuilder as Sequence


class ElementBuilder(Element):

	def __init__(self, category: str) -> None:
		super().__init__()
		self.addAttr("category", category)

	def addContract(self, name: str, value: str) -> "ElementBuilder":
		"""
		Add a contract to the element.
		"""
		element = Element()
		element.addAttrs({"name": name, "value": value})
		self.addElementToNestedSequence(kind="contracts", element=element)
		return self


class SequenceBuilder(Sequence):
	pass

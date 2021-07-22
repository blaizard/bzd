from bzd.parser.element import Element
from bzd.parser.error import Error


class Value:

	def __init__(self, element: Element, kind: str) -> None:
		Error.assertHasAttr(element=element, attr=kind)
		self.element = element
		self.kindAttr = kind

	@property
	def value(self) -> str:
		return self.element.getAttr(self.kindAttr).value

	def __repr__(self) -> str:
		return self.value

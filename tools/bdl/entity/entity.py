from bzd.parser.element import Element


class Entity:

	def __init__(self, element: Element) -> None:
		self.element = element

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

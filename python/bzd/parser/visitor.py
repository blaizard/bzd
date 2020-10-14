import typing

from bzd.parser.element import Sequence, Element
from bzd.parser.parser import ParsedData


class Visitor:
	"""
	Visitor for parsed sequence.

	class Print(Visitor):
		def visitElement(self, element):
			print(element)

	printer = Print()
	printer.visit(data)
	"""

	nestedKind: str = "nested"

	def visit(self, data: typing.Union[Sequence, ParsedData]) -> None:
		sequence = data.data if isinstance(data, ParsedData) else data
		for element in sequence.getList():
			if not element.isEmpty():
				self.visitElement(element)
				nestedSequence = element.getNestedSequence(kind=self.nestedKind)
				if nestedSequence is not None:
					self.visitNestedIn(element)
					self.visit(nestedSequence)
					self.visitNestedOut(element)

	def visitElement(self, element: Element) -> None:
		pass

	def visitNestedIn(self, element: Element) -> None:
		pass

	def visitNestedOut(self, element: Element) -> None:
		pass

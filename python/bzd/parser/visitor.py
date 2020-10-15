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

	nestedKind: typing.Optional[str] = "nested"

	def visit(self, data: typing.Union[Sequence, ParsedData], result: typing.Any = None) -> typing.Any:
		sequence = data.data if isinstance(data, ParsedData) else data
		for element in sequence.getList():
			if not element.isEmpty():
				result = self.visitElement(element, result)
				if self.nestedKind is not None:
					nestedSequence = element.getNestedSequence(kind=self.nestedKind)
					if nestedSequence is not None:
						result = self.visitNested(element, nestedSequence, result)
		return result

	def visitElement(self, element: Element, result: typing.Any) -> typing.Any:
		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.Any) -> typing.Any:
		return self.visit(nestedSequence, result)


class VisitorJson(Visitor):
	"""
	Converts a ParsedData structure into a Json view.
	"""

	nestedKind = None

	def visitElement(self, element: Element, result: typing.Any) -> typing.Any:

		if result is None:
			result = []
		result.append({"attrs": element.getAttrs()})
		for kind, sequence in element.getNestedSequences():
			result[-1][kind] = self.visit(data=sequence, result=[])
		return result

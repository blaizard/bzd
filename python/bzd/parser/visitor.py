import typing
from pathlib import Path

from bzd.parser.element import Sequence, Element
from bzd.parser.error import Error

T = typing.TypeVar("T")
U = typing.TypeVar("U")


class Visitor(typing.Generic[T, U]):
	"""
	Visitor for parsed sequence.

	class Print(Visitor):
		def visitElement(self, element):
			print(element)

	printer = Print()
	printer.visit(data)
	"""

	nestedKind: typing.Optional[str] = "nested"

	def visit(self, sequence: Sequence) -> U:

		result = self._visit(sequence=sequence)
		return self.visitFinal(result=result)

	def _visit(self, sequence: Sequence, result: typing.Any = None) -> T:
		assert isinstance(sequence, Sequence), "Must be a sequence, instead: {}".format(type(sequence))

		result = self.visitBegin(result)

		for element in sequence.iterate():
			result = self.visitElement(element, result)
			if self.nestedKind is not None:
				nestedSequence = element.getNestedSequence(kind=self.nestedKind)
				if nestedSequence is not None:
					result = self.visitNested(element, nestedSequence, result)

		return self.visitEnd(result)

	def visitBegin(self, result: typing.Any) -> typing.Any:
		return result

	def visitEnd(self, result: typing.Any) -> T:
		return typing.cast(T, result)

	def visitFinal(self, result: typing.Any) -> U:
		return typing.cast(U, result)

	def visitElement(self, element: Element, result: typing.Any) -> typing.Any:
		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.Any) -> typing.Any:
		return self._visit(sequence=nestedSequence, result=result)

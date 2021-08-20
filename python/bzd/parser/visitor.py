import typing
from pathlib import Path

from bzd.parser.element import Sequence, Element
from bzd.parser.error import Error

T = typing.TypeVar("T")
U = typing.TypeVar("U")


class _VisitorCommon(typing.Generic[T, U]):

	nestedKind: typing.Optional[str] = "nested"

	def visit(self, sequence: Sequence) -> U:

		result = self._visit(sequence=sequence)  # type: ignore
		return self.visitFinal(result=result)

	def visitEnd(self, result: typing.Any) -> T:
		return typing.cast(T, result)

	def visitFinal(self, result: typing.Any) -> U:
		return typing.cast(U, result)


class Visitor(_VisitorCommon[T, U]):
	"""
	Visitor for parsed sequence.

	class Print(Visitor):
		def visitElement(self, element):
			print(element)

	printer = Print()
	printer.visit(data)
	"""

	def _visit(self, sequence: Sequence, result: typing.Any = None) -> T:
		assert isinstance(sequence, Sequence), "Must be a sequence, instead: {}".format(type(sequence))

		result = self.visitBegin(result)

		for element in sequence:
			result = self.visitElement(element, result)
			if self.nestedKind is not None:
				nestedSequence = element.getNestedSequence(kind=self.nestedKind)
				if nestedSequence is not None:
					result = self.visitNested(element, nestedSequence, result)

		return self.visitEnd(result)

	def visitBegin(self, result: typing.Any) -> typing.Any:
		return result

	def visitElement(self, element: Element, result: typing.Any) -> typing.Any:
		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.Any) -> typing.Any:
		return self._visit(sequence=nestedSequence, result=result)


class VisitorDepthFirst(_VisitorCommon[T, U]):
	"""
	Visitor for parsed sequence.
	"""

	def _visit(self, sequence: Sequence) -> T:
		assert isinstance(sequence, Sequence), "Must be a sequence, instead: {}".format(type(sequence))

		result = self.visitBegin()

		for element in sequence:
			nestedResult: typing.Optional[typing.Any] = None
			if self.nestedKind is not None:
				nestedSequence = element.getNestedSequence(kind=self.nestedKind)
				if nestedSequence is not None:
					nestedResult = self.visitNested(element, nestedSequence)
			result = self.visitElement(element, result, nestedResult)

		return self.visitEnd(result)

	def visitBegin(self) -> typing.Any:
		return None

	def visitNested(self, element: Element, nestedSequence: Sequence) -> typing.Any:
		return self._visit(sequence=nestedSequence)

	def visitElement(self, element: Element, result: typing.Any, nestedResult: typing.Any) -> typing.Any:
		return result

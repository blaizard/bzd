import typing
import sys

from pathlib import Path
from bzd.parser.element import Element
from bzd.parser.context import Context


class ExceptionParser(Exception):

	def __init__(self, message: str) -> None:
		super().__init__(message)


class Error:
	"""
	Handle errors.
	"""

	@staticmethod
	def toString(context: typing.Optional[Context], index: int, message: str) -> str:

		# Look for the content
		if context is None:
			return message
		if context.content is None:
			if context.path is None:
				return message
			content = context.path.read_text()
		else:
			content = context.content
		contentByLine = content.split("\n")

		# Identify the line and column
		current = 0
		line = 0
		for contentLine in contentByLine:
			current += len(contentLine) + 1
			if current > index:
				break
			line += 1
		column = len(contentByLine[line]) - (current - index) + 1

		# Position the cursor
		contentByLine.insert(line + 1, "{}^".format(" " * column))
		contentByLine.insert(
			line + 2, "{}:{}:{}: error: {}".format("<string>" if context.path is None else context.path, line + 1,
			column + 1, message))

		return "\n" + "\n".join(contentByLine[max(0, line-1):line+3])

	@staticmethod
	def toStringFromElement(element: Element, attr: typing.Optional[str] = None, message: str = "Error") -> str:

		# Look for the index
		index = 0
		if attr is not None and element.isAttr(attr):
			index = element.getAttr(attr).index

		# Use the begining of the element
		else:
			startIndex = sys.maxsize
			for key, attrObj in element.getAttrs().items():
				startIndex = min(startIndex, attrObj.index)
			if startIndex < sys.maxsize:
				index = startIndex

		return Error.toString(context=element.context, index=index, message=message)

	@staticmethod
	def handle(context: Context, index: int, message: str) -> None:
		raise ExceptionParser(message=Error.toString(context=context, index=index, message=message))

	@staticmethod
	def handleFromElement(element: Element, attr: typing.Optional[str] = None, message: str = "Error") -> None:
		raise ExceptionParser(message=Error.toStringFromElement(element=element, attr=attr, message=message))

	@staticmethod
	def assertTrue(element: Element, condition: bool, message: str, attr: typing.Optional[str] = None) -> None:
		"""
		Ensures a specific condition evaluates to True.
		"""

		if not condition:
			Error.handleFromElement(element=element, attr=attr, message=message)

	@staticmethod
	def assertHasAttr(element: Element, attr: str) -> None:
		"""
		Ensures an element has a specific attribute.
		"""

		if not element.isAttr(attr):
			Error.handleFromElement(element=element, attr=None, message="Mising mandatory attribute '{}'.".format(attr))

	@staticmethod
	def assertHasSequence(element: Element, sequence: str) -> None:
		"""
		Ensures an element has a specific sequence.
		"""

		if not element.isNestedSequence(sequence):
			Error.handleFromElement(element=element,
				attr=None,
				message="Mising mandatory sequence '{}'.".format(sequence))

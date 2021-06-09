import typing
import sys

from pathlib import Path
from bzd.parser.element import Element


class ExceptionParser(Exception):

	def __init__(self, message: str) -> None:
		super().__init__(message)


class Error:
	"""
	Handle errors.
	"""
	path_: typing.Optional[Path] = None
	content_: typing.Optional[str] = None

	@staticmethod
	def setContext(path: typing.Optional[Path] = None, content: typing.Optional[str] = None) -> None:
		Error.path_ = path
		Error.content_ = content

	@staticmethod
	def handle(index: int, message: str) -> None:

		# Look for the content
		if Error.content_ is None:
			if Error.path_ is None:
				raise ExceptionParser(message=message)
			content = Error.path_.read_text()
		else:
			content = Error.content_
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
			line + 2, "{}:{}:{}: error: {}".format("<string>" if Error.path_ is None else Error.path_, line + 1,
			column + 1, message))

		raise ExceptionParser(message="\n" + "\n".join(contentByLine))

	@staticmethod
	def handleFromElement(element: Element, attr: typing.Optional[str] = None, message: str = "Error") -> None:

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

		Error.handle(index=index, message=message)

	@staticmethod
	def assertTrue(element: Element, condition: bool, message: str) -> None:
		"""
		Ensures a specific condition evaluates to True.
		"""

		if not condition:
			Error.handleFromElement(element=element, attr=None, message=message)

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

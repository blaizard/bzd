import typing
import logging
import sys
import re

from pathlib import Path
from bzd.parser.element import Element
from bzd.parser.context import Context

useColors: bool = False


class ExceptionParser(Exception):

	def __init__(self, message: str) -> None:
		super().__init__(message)


class Error:
	"""
	Handle errors.
	"""

	@staticmethod
	def toString(context: typing.Optional[Context], index: int, end: int, message: str) -> str:

		# Look for the content
		if context is None:
			return message
		if context.content is None:
			if context.path is None:
				return message
			try:
				content = context.path.read_text()
			except:
				logging.warning("Could not open file '{}'.".format(context.path.as_posix()))
				return message
		else:
			content = context.content
		contentByLine = content.split("\n")

		# Identify the line and column
		line = 0
		endLine = 0

		# Look for the line where the error starts
		current = 0
		for contentLine in contentByLine:
			current += len(contentLine) + 1
			if current > index:
				break
			line += 1
		column = len(contentByLine[line]) - (current - index) + 1

		# Look for the line where the error ends
		current = 0
		for contentLine in contentByLine:
			current += len(contentLine) + 1
			if current > end:
				break
			endLine += 1

		# Set colors if any
		global useColors
		colorBegin = "\033[01;31m" if useColors else ""
		colorEnd = "\033[0m" if useColors else ""

		# Position the cursor.
		# A padding must be created to correctly print special spaces such as tabs for example.
		paddingLine = re.sub(r"[^\s]", " ", contentByLine[line])
		contentByLine.insert(line + 1, "{padding}^".format(padding=paddingLine[0:column]))
		contentByLine.insert(
			line + 2, "{path}:{line}:{column}: {colorBegin}error: {message}{colorEnd}".format(
			path="<string>" if context.path is None else context.path,
			line=line + 1,
			column=column + 1,
			colorBegin=colorBegin,
			message=message,
			colorEnd=colorEnd))

		return "\n" + "\n".join(contentByLine[max(0, line - 1):endLine + 3])

	@staticmethod
	def toStringFromElement(element: Element, attr: typing.Optional[str] = None, message: str = "Error") -> str:

		# Look for the index
		index = 0
		end = 0
		if attr is not None and element.isAttr(attr):
			index = element.getAttr(attr).index
			end = element.getAttr(attr).end

		# Use the begining of the element
		else:
			startIndex = sys.maxsize
			endIndex = 0
			for key, attrObj in element.getAttrs().items():
				startIndex = min(startIndex, attrObj.index)
				endIndex = max(endIndex, attrObj.end)
			if startIndex < sys.maxsize:
				index = startIndex
				end = endIndex

		return Error.toString(context=element.context, index=index, end=end, message=message)

	@staticmethod
	def handle(context: Context, index: int, end: int, message: str) -> None:
		raise ExceptionParser(message=Error.toString(context=context, index=index, end=end, message=message))

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

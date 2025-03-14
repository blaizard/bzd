import typing
import logging
import sys
import re
from pathlib import Path

from bzd.utils.result import Result as ResultBase

from bzd.parser.element import Element
from bzd.parser.fragments import IGNORE_INDEX_VALUE
from bzd.parser.context import Context

useColors: bool = False

V = typing.TypeVar("V")


class Result(ResultBase[V, str]):

	def assertValue(self, element: Element, attr: typing.Optional[str] = None) -> V:
		"""
        Ensure the result holds a value, else print the error and throw.
        """
		if not bool(self):
			Error.handleFromElement(element=element, attr=attr, message=self.error)
		return self.value


class ExceptionParser(Exception):

	def __init__(self, message: str) -> None:
		super().__init__(message)


class AssertionResult:
	"""Type returned by an assertion."""

	def __init__(self, message: typing.Optional[str] = None) -> None:
		self.message = message

	def __bool__(self) -> bool:
		return isinstance(self.message, str)

	def extend(
	    self,
	    element: Element,
	    message: str,
	    attr: typing.Optional[str] = None,
	    throw: bool = True,
	) -> "AssertionResult":
		"""Extend an assertion message if any."""

		if bool(self):
			self.message += Error.toStringFromElement(element=element, attr=attr, message=message)  # type: ignore
			if throw:
				raise ExceptionParser(message=self.message)

		return self

	def __str__(self) -> str:
		return self.message if bool(self) else ""  # type: ignore


class Error:
	"""
    Handle errors.
    """

	@staticmethod
	def valueExtract(value: typing.Any) -> str:
		"""Print a snapshot of the value."""
		valueStr = str(value)
		if len(valueStr) > 1024:
			return f"{valueStr[:1024]}[...]"
		return valueStr

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
		line = min(len(contentByLine) - 1, line)
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
		    line + 2,
		    "{path}:{line}:{column}: {colorBegin}error: {message}{colorEnd}".format(
		        path="<string>" if context.path is None else context.path,
		        line=line + 1,
		        column=column + 1,
		        colorBegin=colorBegin,
		        message=message,
		        colorEnd=colorEnd,
		    ),
		)

		return "\n" + "\n".join(contentByLine[max(0, line - 1):endLine + 3])

	@staticmethod
	def toStringFromElement(element: Element, attr: typing.Optional[str] = None, message: str = "Error") -> str:
		context, start, end = element.makeContext(attr=attr)
		message += f"\nIn context of {element.toString(nested = False, simplified = True)}"
		return Error.toString(context=context, index=start, end=end, message=message)

	@staticmethod
	def handle(context: Context, index: int, end: int, message: str, throw: bool = True) -> str:
		message = Error.toString(context=context, index=index, end=end, message=message)
		if throw:
			raise ExceptionParser(message=message)
		return message

	@staticmethod
	def handleFromElement(
	    element: Element,
	    attr: typing.Optional[str] = None,
	    message: str = "Error",
	    throw: bool = True,
	) -> AssertionResult:
		message = Error.toStringFromElement(element=element, attr=attr, message=message)
		if throw:
			raise ExceptionParser(message=message)
		return AssertionResult(message)

	@staticmethod
	def assertTrue(
	    element: Element,
	    condition: bool,
	    message: str,
	    attr: typing.Optional[str] = None,
	    throw: bool = True,
	) -> AssertionResult:
		"""
        Ensures a specific condition evaluates to True.
        """

		if not condition:
			return Error.handleFromElement(element=element, attr=attr, message=message, throw=throw)
		return AssertionResult()

	@staticmethod
	def assertHasAttr(element: Element, attr: str, throw: bool = True) -> AssertionResult:
		"""
        Ensures an element has a specific attribute.
        """

		if not element.isAttr(attr):
			return Error.handleFromElement(
			    element=element,
			    attr=None,
			    message="Missing mandatory attribute '{}'.".format(attr),
			    throw=throw,
			)
		return AssertionResult()

	@staticmethod
	def assertHasSequence(element: Element, sequence: str, throw: bool = True) -> AssertionResult:
		"""
        Ensures an element has a specific sequence.
        """

		if not element.isNestedSequence(sequence):
			return Error.handleFromElement(
			    element=element,
			    attr=None,
			    message="Missing mandatory sequence '{}'.".format(sequence),
			    throw=throw,
			)
		return AssertionResult()

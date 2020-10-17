import typing
import sys

from bzd.parser.element import Element
if typing.TYPE_CHECKING:
	from bzd.parser.parser import Parser


def handleError(parser: "Parser", index: int, message: str) -> None:
	contentByLine = parser.content.split("\n")
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
	contentByLine.insert(line + 2, "{}:{}:{}: error: {}".format(parser.path, line + 1, column + 1, message))
	print("\n".join(contentByLine))
	raise Exception()


def handleFromElement(element: Element, attr: typing.Optional[str], message: str) -> None:

	# Look for the index
	index = 0
	if attr is not None and element.isAttr(attr):
		index = element.getAttrs()[attr].index

	# Use the begining of the element
	else:
		startIndex = sys.maxsize
		for key, attrObj in element.getAttrs().items():
			startIndex = min(startIndex, attrObj.index)
		if startIndex < sys.maxsize:
			index = startIndex

	handleError(parser=element.parser, index=index, message=message)


def assertHasAttr(element: Element, attr: str) -> None:
	"""
	Ensures an element has a specific attribute.
	"""

	if not element.isAttr(attr):
		handleFromElement(element=element, attr=None, message="Mising mandatory attribute '{}'.".format(attr))


def assertHasSequence(element: Element, sequence: str) -> None:
	"""
	Ensures an element has a specific sequence.
	"""

	if not element.isNestedSequence(sequence):
		handleFromElement(element=element, attr=None, message="Mising mandatory sequence '{}'.".format(sequence))

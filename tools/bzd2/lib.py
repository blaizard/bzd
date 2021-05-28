import typing
from pathlib import Path

from bzd.parser.element import Sequence
from tools.bzd2.format.bdl import BdlFormatter
from tools.bzd2.format.cc import CcFormatter
from tools.bzd2.grammar import Parser

formatters = {"bdl": BdlFormatter, "cc": CcFormatter}


def main(formatType: str, inputs: typing.List[Path]) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Parse all files and merge them
	data = Sequence()
	for path in inputs:
		parsedData = Parser(path).parse()
		data.merge(parsedData)

	# Format using a specific formatter
	formatter = formatters[formatType]()
	return formatter.visit(data)  # type: ignore

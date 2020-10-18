import typing
from pathlib import Path

from bzd.parser.element import Sequence
from tools.bzd2.format.bdl import BdlFormatter
from tools.bzd2.format.cc import CcFormatter
from tools.bzd2.grammar import Parser

formatters = {"bdl": BdlFormatter, "cc": CcFormatter}


def main(format: str, inputs: typing.List[Path]) -> str:

	assert format in formatters, "Format '{}' not supported.".format(format)

	# Parse all files and merge them
	data = Sequence()
	for path in inputs:
		parsedData = Parser(path).parse()
		data.merge(parsedData)

	# Format using a specific formatter
	formatter = formatters[format]()
	return formatter.visit(data)

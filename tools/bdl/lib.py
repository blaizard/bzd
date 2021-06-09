import typing
import json
from pathlib import Path

from bzd.parser.element import Sequence
from bzd.parser.error import Error

from tools.bdl.lang.bdl.visitor import formatBdl
from tools.bdl.lang.cc.visitor import formatCc
from tools.bdl.grammar import Parser
from tools.bdl.visitors.result import Result
from tools.bdl.visitors.validation import Validation
from tools.bdl.visitors.map import Map

formatters = {"bdl": formatBdl, "cc": formatCc}


def main(formatType: str, path: Path) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Parse the input file
	data = Parser.fromPath(path).parse()

	# Validation step
	Validation().visit(data)

	# Generate the symbol map
	symbols = Map().visit(data)

	# Save the preprocessed files
	preprocessed = {"parsed": data.serialize(), "symbols": symbols}
	preprocessedStr = json.dumps(preprocessed, separators=(",", ":"))
	print(preprocessedStr)

	Error.setContext(path=path)

	# Process the result
	result = Result(symbols=symbols).visit(data)

	# Format using a specific formatter
	return formatters[formatType](result)

import typing
import json
from pathlib import Path

from bzd.parser.element import Sequence
from tools.bdl.lang.bdl.visitor import BdlFormatter
from tools.bdl.lang.cc.visitor import CcFormatter
from tools.bdl.grammar import Parser
from tools.bdl.visitors.validation import Validation
from tools.bdl.visitors.map import Map

formatters = {"bdl": BdlFormatter, "cc": CcFormatter}


def main(formatType: str, path: Path) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Parse the input file
	data = Parser.fromPath(path).parse()

	# Validation step
	Validation().visit(data)

	# Generate the symbol map
	symbolMap = Map().visit(data)

	# Save the preprocessed files
	preprocessed = {"parsed": data.serialize(), "symbols": symbolMap}
	preprocessedStr = json.dumps(preprocessed, separators=(",", ":"))
	print(preprocessedStr)

	# Format using a specific formatter
	formatter = formatters[formatType]()
	return formatter.visit(data)

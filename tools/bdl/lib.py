import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.lang.bdl.visitor import formatBdl
from tools.bdl.lang.cc.visitor import formatCc
from tools.bdl.visitors.result import Result
from tools.bdl.object import Object

formatters = {"bdl": formatBdl, "cc": formatCc}


def main(formatType: str, path: Path) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Parse the input file
	bdl = Object.fromPath(path=path)

	Error.setContext(path=path)

	# Process the result
	result = Result(symbols=bdl.symbols).visit(bdl.parsed)

	print(result)

	# Format using a specific formatter
	return formatters[formatType](result)

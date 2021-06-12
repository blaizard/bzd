import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.lang.bdl.visitor import formatBdl
from tools.bdl.lang.cc.visitor import formatCc
from tools.bdl.visitors.result import Result
from tools.bdl.object import Object

formatters = {"bdl": formatBdl, "cc": formatCc}

def preprocess(path: Path) -> Object:

	# Parse the input file
	return Object.fromPath(path=path)

def generate(formatType: str, bdl: Object) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Process the result
	result = Result(bdl=bdl).process()

	print(result)

	# Format using a specific formatter
	return formatters[formatType](result)

def main(formatType: str, path: Path) -> str:

	bdl = preprocess(path=path)

	# Set context for the error handling
	Error.setContext(path=path)

	return generate(formatType=formatType, bdl=bdl)

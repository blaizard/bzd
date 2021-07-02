import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.generators.bdl.visitor import formatBdl
from tools.bdl.generators.cc.visitor import formatCc
from tools.bdl.visitors.result import Result
from tools.bdl.object import Object

formatters = {"bdl": formatBdl, "cc": formatCc}


def preprocess(path: Path, use_path: typing.Optional[typing.List[Path]] = None) -> Object:

	# Parse the input file
	return Object.fromPath(path=path, use_path=[] if use_path else use_path)


def generate(formatType: str, bdl: Object) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Format using a specific formatter
	return formatters[formatType](bdl=bdl)


def main(formatType: str, path: Path, use_path: typing.Optional[typing.List[Path]] = None) -> str:

	bdl = preprocess(path=path, use_path=[] if use_path else use_path)
	return generate(formatType=formatType, bdl=bdl)

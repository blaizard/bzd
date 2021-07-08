import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.generators.bdl.visitor import formatBdl
from tools.bdl.generators.cc.visitor import formatCc
from tools.bdl.object import Object, ObjectContext

formatters = {"bdl": formatBdl, "cc": formatCc}


def preprocess(path: Path, objectContext: typing.Optional[ObjectContext] = None) -> Object:

	objectContext = objectContext if objectContext is not None else ObjectContext()
	assert objectContext

	# Preprocess the object
	return objectContext.preprocess(path=path)


def generate(formatType: str, bdl: Object) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Format using a specific formatter
	return formatters[formatType](bdl=bdl)


def main(formatType: str, path: Path, objectContext: typing.Optional[ObjectContext] = None) -> str:

	bdl = preprocess(path=path, objectContext=objectContext)
	return generate(formatType=formatType, bdl=bdl)

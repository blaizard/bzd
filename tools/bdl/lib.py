import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.visitors.composition.visitor import Composition
from tools.bdl.generators.bdl.visitor import formatBdl
from tools.bdl.generators.cc.visitor import formatCc, compositionCc
from tools.bdl.object import Object, ObjectContext

formatters = {"bdl": formatBdl, "cc": formatCc}
compositions = {"cc": compositionCc}


def preprocess(source: str, objectContext: typing.Optional[ObjectContext] = None) -> Object:

	objectContext = objectContext if objectContext is not None else ObjectContext()
	assert objectContext

	# Preprocess the object
	return objectContext.preprocess(source=source)


def generate(formatType: str, bdl: Object, includes: typing.List[Path]) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Format using a specific formatter
	return formatters[formatType](bdl=bdl, includes=includes)


def compose(formatType: str, bdls: typing.Sequence[Object], output: Path, includes: typing.List[Path]) -> None:

	composition = Composition(includes=includes)
	for bdl in bdls:
		composition.visit(bdl)
	composition.process()

	# Generate the composition using a specific formatter
	data = compositions[formatType](composition=composition)

	output.write_text(data)


def main(formatType: str,
	source: str,
	includes: typing.List[Path],
	objectContext: typing.Optional[ObjectContext] = None) -> str:

	bdl = preprocess(source=source, objectContext=objectContext)
	return generate(formatType=formatType, bdl=bdl, includes=includes)

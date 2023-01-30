import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.visitors.composition.visitor import Composition
from tools.bdl.generators.bdl.visitor import formatBdl
from tools.bdl.generators.cc.visitor import formatCc, compositionCc
from tools.bdl.object import Object, ObjectContext

formatters = {"bdl": formatBdl, "cc": formatCc}
compositions = {"cc": compositionCc}


def preprocess(source: str,
	namespace: typing.Optional[str] = None,
	objectContext: typing.Optional[ObjectContext] = None) -> Object:

	objectContext = objectContext if objectContext is not None else ObjectContext()
	assert objectContext

	# Preprocess the object
	return objectContext.preprocess(source=source, namespace=namespace)


def generate(formatType: str, bdl: Object, data: typing.Optional[Path] = None) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Format using a specific formatter
	return formatters[formatType](bdl=bdl, data=data)


def compose(formatType: str, bdls: typing.Sequence[Object], output: Path, data: typing.Optional[Path] = None) -> None:

	composition = Composition()
	for bdl in bdls:
		composition.visit(bdl)
	composition.process()

	# Generate the composition using a specific formatter
	content = compositions[formatType](composition=composition, data=data)

	output.write_text(content)


def main(formatType: str,
	source: str,
	objectContext: typing.Optional[ObjectContext] = None,
	data: typing.Optional[Path] = None) -> str:

	bdl = preprocess(source=source, objectContext=objectContext)
	return generate(formatType=formatType, bdl=bdl, data=data)

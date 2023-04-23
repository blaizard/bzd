import typing
from pathlib import Path

from bzd.parser.error import Error

from tools.bdl.visitors.composition.visitor import Composition
from tools.bdl.generators.bdl.visitor import formatBdl
from tools.bdl.generators.cc.visitor import formatCc, compositionCc
from tools.bdl.object import Object, ObjectContext

formatters = {"bdl": formatBdl, "cc": formatCc}
compositions_ = {"cc": compositionCc}


def preprocess(source: str,
               namespace: typing.Optional[typing.List[str]] = None,
               objectContext: typing.Optional[ObjectContext] = None) -> Object:

	objectContext = objectContext if objectContext is not None else ObjectContext()
	assert objectContext

	# Preprocess the object
	return objectContext.preprocess(source=source, namespace=namespace)


def generate(formatType: str, bdl: Object, data: typing.Optional[Path] = None) -> str:

	assert formatType in formatters, "Format '{}' not supported.".format(formatType)

	# Format using a specific formatter
	return formatters[formatType](bdl=bdl, data=data)


def compose(formatType: str,
            bdls: typing.Sequence[Object],
            output: Path,
            targets: typing.Set[str],
            data: typing.Optional[Path] = None) -> None:

	composition = Composition(targets=targets)
	for bdl in bdls:
		composition.visit(bdl)
	composition.process()

	# Generate the composition views using a specific formatter and call the language-specific composition.
	compositions = {target: composition.view(target) for target in targets}
	compositions_[formatType](compositions=compositions, data=data, output=output)


def main(formatType: str,
         source: str,
         objectContext: typing.Optional[ObjectContext] = None,
         data: typing.Optional[Path] = None) -> str:

	bdl = preprocess(source=source, objectContext=objectContext)
	return generate(formatType=formatType, bdl=bdl, data=data)

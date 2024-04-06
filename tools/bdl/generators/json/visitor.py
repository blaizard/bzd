import typing
from pathlib import Path

from tools.bdl.object import Object
from tools.bdl.visitors.composition.visitor import CompositionView


def formatJson(bdl: Object, data: typing.Optional[Path] = None) -> str:
	return ""


def compositionJson(
    compositions: typing.Dict[str, CompositionView],
    output: Path,
    data: typing.Optional[Path] = None,
) -> None:

	for target, composition in compositions.items():

		data = {}

		(output.parent / f"{output.name}.{target}.json").write_text("{}")
		print(composition)

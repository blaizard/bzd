import argparse
import pathlib
import json
import typing

from config.reader import internalToKeyData, Data

from bzd.utils.runfiles import pathFromRLocation
from bzd.utils.secret import decrypt


def keyDayaToStr(data: typing.Any) -> str:

	def mergeLinesWithPrefix(prefix, lines, hasNewLine) -> None:
		if not hasNewLine and len(lines) == 1:
			return [f"{prefix} {lines[0].lstrip()}"]
		else:
			return [prefix, *lines]

	def recursive(value: typing.Any, indent: int) -> typing.Tuple[typing.List[str], bool]:
		lines = []
		pad = "   " * indent

		metadata = value.metadata if isinstance(value, Data) else []
		rawValue = value.value if isinstance(value, Data) else value
		requiresNewLine = False

		if "secret" in metadata:
			rawValue = decrypt(rawValue)

		if "path" in metadata:
			rawValue = pathFromRLocation(rawValue)

		if isinstance(rawValue, dict):
			for key, data in rawValue.items():
				key = data.key if isinstance(data, Data) else key
				content, hasNewLine = recursive(value=data, indent=indent + 1)
				lines.extend(mergeLinesWithPrefix(f"{pad}{key}:", content, hasNewLine))
			requiresNewLine = True

		elif isinstance(rawValue, list):
			for item in rawValue:
				content, _hasNewLine = recursive(value=item, indent=indent + 1)
				lines.append(f"{pad} - {content[0].lstrip()}")
				lines.extend(content[1:])
			requiresNewLine = True

		elif isinstance(rawValue, (int, float, bool, str)):
			lines.append(f"{pad}{json.dumps(rawValue)}")

		else:
			lines.append(f"{pad} <{type(rawValue).__name__}> {str(rawValue)}")

		if metadata:
			lines = mergeLinesWithPrefix(f"{pad}[{', '.join(metadata)}]", lines, requiresNewLine)

		return lines, requiresNewLine

	lines, _ = recursive(value=data, indent=0)
	return "\n".join(lines)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Visualize the configuration.")
	parser.add_argument(
		"--internal",
		type=pathlib.Path,
		help="The internal values in json format.",
		required=True,
	)

	args = parser.parse_args()
	internal = json.loads(args.internal.read_text())

	output = internalToKeyData(internal)
	content = keyDayaToStr(output.nested)
	print(content)

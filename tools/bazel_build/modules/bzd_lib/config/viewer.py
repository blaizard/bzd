import argparse
import pathlib
import json
import typing

from config.reader import internalToKeyData, Data


def keyDayaToStr(values: typing.Any, indent: int = 0) -> str:

	lines = []
	pad = "   " * indent

	if isinstance(values, dict):
		for key, data in values.items():
			value = data.value if isinstance(data, Data) else data
			key = data.key if isinstance(data, Data) else key

			if isinstance(value, (dict, list)):
				lines.append(f"{pad}{key}:")
				lines.append(keyDayaToStr(values=value, indent=indent + 1))
			else:
				lines.append(f"{pad}{key}: {json.dumps(value)}")

	elif isinstance(values, list):
		for item in values:
			if isinstance(item, dict):
				subLines = keyDayaToStr(values=item, indent=indent + 1).split("\n")
				subLines[0] = f"{pad} - {subLines[0].lstrip()}"
				lines.append("\n".join(subLines))
			elif isinstance(item, list):
				lines.append(f"{pad} -")
				lines.append(keyDayaToStr(values=item, indent=indent + 1))
			else:
				lines.append(f"{pad} - {json.dumps(item)}")

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

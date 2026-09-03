import argparse
import pathlib
import typing
import json

from config.reader import internalToKeyData


def toPython(data: typing.Any) -> typing.Tuple[str, str]:
	"""Convert data to Python format."""

	if isinstance(data, bool):
		return ("True" if data else "False"), "bool"
	if isinstance(data, str):
		return json.dumps(data), "str"
	if isinstance(data, int):
		return str(data), "int"
	if isinstance(data, float):
		return str(data), "float"
	if data is None:
		return "None", "typing.Any"
	if isinstance(data, list):
		content = ", ".join([toPython(v)[0] for v in data])
		return f"[ {content} ]", "list"
	if isinstance(data, dict):
		content = ", ".join([f'"{k}": {toPython(v)[0]}' for k, v in data.items()])
		return f"{{ {content} }}", "dict"
	return str(data), "typing.Any"


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Generate Python bindings.")
	parser.add_argument("--output", type=pathlib.Path, required=True, help="The output path for the generated file.")
	parser.add_argument(
		"--internal",
		type=pathlib.Path,
		help="The internal values in json format.",
		required=True,
	)
	parser.add_argument(
		"--expose",
		type=json.loads,
		help="The configuration keys to expose.",
		required=True,
	)

	args = parser.parse_args()
	internal = json.loads(args.internal.read_text())

	content = """# This file was auto-generated.
# mypy: ignore-errors

import pathlib
import typing

from bzd.utils.runfiles import pathFromRLocation
from bzd.utils.secret import decrypt

ConfigValue = typing.Callable[[], typing.Any]

"""
	processed = {}
	for key, data in internalToKeyData(internal).flat.items():
		returnType = "typing.Any"
		if isinstance(data.value, dict):
			entries = [f"{json.dumps(v.key)}: _config[{json.dumps(k)}]()" for k, v in data.value.items()]
			value = "{ " + ", ".join(entries) + " }"
			returnType = "typing.Dict[str, typing.Any]"
		else:
			value, returnType = toPython(data.value)

		if "path" in data.metadata:
			value = f"pathFromRLocation({value})"
			returnType = "pathlib.Path"
		if "secret" in data.metadata:
			value = f"decrypt({value})"
			returnType = "str"

		processed[key] = (value, returnType)

	# Generate the internal config object.
	content += "_config: typing.Dict[str, ConfigValue] = {\n"
	for key, (value, returnType) in processed.items():
		content += f"\t{json.dumps(key)}: lambda: {value},\n"
	content += "}\n\n"

	# Generate the exposed config object.
	for key, exposedKey in args.expose.items():
		if key not in processed:
			raise ValueError(f"Key '{key}' not found in config.")
		_, returnType = processed[key]
		content += f"{exposedKey}: typing.Callable[[], {returnType}] = _config[{json.dumps(key)}]\n"

	args.output.write_text(content)

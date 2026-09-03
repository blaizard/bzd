import argparse
import pathlib
import typing
import json

from config.reader import internalToKeyData


def toTypeScript(data: typing.Any) -> typing.Tuple[str, str]:
	"""Convert data to TypeScript format."""

	if isinstance(data, bool):
		return ("true" if data else "false"), "boolean"
	if isinstance(data, str):
		return json.dumps(data), "string"
	if isinstance(data, int):
		return str(data), "number"
	if isinstance(data, float):
		return str(data), "number"
	if data is None:
		return "null", "null"
	if isinstance(data, list):
		content = ", ".join([toTypeScript(v)[0] for v in data])
		return f"[ {content} ]", "unknown[]"
	if isinstance(data, dict):
		content = ", ".join([f'"{k}": {toTypeScript(v)[0]}' for k, v in data.items()])
		return f"{{ {content} }}", "object"
	return json.dumps(data), "unknown"


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Generate nodejs bindings.")
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

	content = """// This file was auto-generated.

// @ts-ignore TS6133
import { pathFromRLocation } from "#bzd/nodejs/utils/runfiles.js"
// @ts-ignore TS6133
import { decrypt } from "#bzd/nodejs/utils/secret.js"

type ConfigValue = () => unknown;

"""
	processed = {}
	for key, data in internalToKeyData(internal).flat.items():
		returnType = "unknown"
		if isinstance(data.value, dict):
			entries = [f"{json.dumps(v.key)}: config[{json.dumps(k)}]()" for k, v in data.value.items()]
			value = "{ " + ", ".join(entries) + " }"
			returnType = "Record<string, unknown>"
		else:
			value, returnType = toTypeScript(data.value)

		if "path" in data.metadata:
			value = f"pathFromRLocation({value})"
			returnType = "string"
		if "secret" in data.metadata:
			value = f"decrypt({value})"
			returnType = "string"

		processed[key] = (value, returnType)

	# Generate the internal config object.
	content += "const config: Record<string, ConfigValue> = {\n"
	for key, (value, returnType) in processed.items():
		content += f"\t{json.dumps(key)}: () => {{ return {value}; }},\n"
	content += "};\n\n"

	# Generate the exposed config object.
	for key, exposedKey in args.expose.items():
		if key not in processed:
			raise ValueError(f"Key '{key}' not found in config.")
		_, returnType = processed[key]
		content += f"export const {exposedKey}: () => {returnType} = config[{json.dumps(key)}] as () => {returnType};\n"

	args.output.write_text(content)

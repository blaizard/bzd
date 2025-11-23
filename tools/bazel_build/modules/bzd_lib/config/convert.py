import argparse
import pathlib
import typing
import json
import re
import yaml

from config.reader import processKeyValues, internalToDictionary


def toJson(data: typing.Any) -> typing.Any:
	return json.dumps(data)


def toPython(data: typing.Any) -> typing.Any:
	if data is None:
		return "None"
	if isinstance(data, str):
		return f"\"{data}\""
	if isinstance(data, dict):
		content = ", ".join([f"\"{k}\": {toPython(v)}" for k, v in data.items()])
		return f"{{ {content} }}"
	if isinstance(data, list):
		content = ", ".join([toPython(v) for v in data])
		return f"[ {content} ]"
	return str(data)


def toSubsetChar(name: str) -> str:
	return re.sub(r"[^a-zA-Z0-9_]+", "_", name)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Substitute template values.")
	parser.add_argument("--output", type=pathlib.Path, help="The output path for the generated file.")
	parser.add_argument("--json", type=pathlib.Path, help="The internal values in json format.", required=True)
	parser.add_argument("--format", choices=["yaml", "py", "json"], type=str, help="The output format.", required=True)

	args = parser.parse_args()
	data = internalToDictionary(json.loads(args.json.read_text()))

	if args.format == "py":
		content = """# This file was auto-generated.
# mypy: ignore-errors

"""
		for key, value in processKeyValues(data).items():
			content += f"{toSubsetChar(key)} = {toPython(value)}\n"

	elif args.format == "yaml":
		content = yaml.dump(data)

	elif args.format == "json":
		content = json.dumps(data, indent=4)

	else:
		raise Exception(f"Unsupported format: {args.format}.")

	if args.output:
		args.output.write_text(content)
	else:
		print(content)

import argparse
import pathlib
import json
import typing
import sys


def processKeyValues(data: typing.Dict[str, typing.Any]) -> typing.Dict[str, typing.Any]:
	"""Gather all the key/values."""

	values = {}
	for key, value in data.items():
		if isinstance(value, dict):
			for nestedKey, nestedValue in Config.processKeyValues(value).items():
				values[f"{key}.{nestedKey}"] = nestedValue
		else:
			values[key] = value
	return values


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Configuration reader.")
	parser.add_argument("config", type=pathlib.Path, help="Path of the configuration file.")
	parser.add_argument(
	    "keys",
	    nargs="*",
	    type=str,
	    help="Keys to be read, the result outputs 1 value per line in the same order.",
	)
	args = parser.parse_args()

	config = json.loads(args.config.read_text())
	values = processKeyValues(config)

	for key in args.keys:
		assert key in values, f"Key: '{key}' is not valid within the configuration."
		print(values[key])

	sys.exit(0)

import argparse
import pathlib
import json
import typing
import sys

from bzd.utils.dict import updateDeep, UpdatePolicy

InternalFragment = typing.Tuple[typing.Optional[str], typing.Any, str]
InternalFragmentList = typing.List[InternalFragment]


def processKeyValues(data: typing.Dict[str, typing.Any]) -> typing.Dict[str, typing.Any]:
	"""Gather all the key/values."""

	values = {}
	for key, value in data.items():
		if isinstance(value, dict):
			for nestedKey, nestedValue in processKeyValues(value).items():
				values[f"{key}.{nestedKey}"] = nestedValue
		else:
			values[key] = value
	return values


def internalToDictionary(internal: InternalFragmentList) -> typing.Dict[str, typing.Any]:
	"""Convert the internal representation of the configuration into a dictionary."""

	output: typing.Dict[str, typing.Any] = {}
	for keyStr, value, source in internal:
		data = makeDictionary(keyStr, value)
		updateDeep(output, data, UpdatePolicy.override)
	return output


def makeDictionary(keyStr: typing.Optional[str], value: typing.Any) -> typing.Dict[str, typing.Any]:
	"""Make a dictionary from a key string.
	
	If the key string is None, return the value directly.
	"""

	if keyStr is None:
		assert isinstance(value, dict), "Value must be a dictionary if keyStr is None."
		return value

	root: typing.Dict[str, typing.Any] = {}
	data = root
	[*keys, key] = keyStr.split(".")
	for k in keys:
		data = data.setdefault(k, {})
	data[key] = value

	return root


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

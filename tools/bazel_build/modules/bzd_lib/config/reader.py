import argparse
import pathlib
import json
import typing
import sys
import dataclasses

from bzd.utils.dict import updateDeep, UpdatePolicy

Metadata = typing.List[str]
InternalFragment = typing.Tuple[typing.Optional[str], typing.Any, Metadata, str]
InternalFragmentList = typing.List[InternalFragment]


@dataclasses.dataclass
class Data:
	value: typing.Any
	metadata: Metadata


def processKeyData(
	data: typing.Dict[str, typing.Any],
	metadata: typing.Dict[str, typing.Any],
) -> typing.Dict[str, Data]:
	"""Gather all the key/values."""

	values = {}
	for key, value in data.items():
		if isinstance(value, dict):
			value = processKeyData(value, metadata)
			for nestedKey, nestedValue in value.items():
				values[f"{key}.{nestedKey}"] = nestedValue

		values[key] = Data(
			value=value,
			metadata=metadata.get(key, []),
		)

	return values


def internalToKeyData(
	internal: InternalFragmentList,
) -> typing.Dict[str, Data]:
	"""Convert the internal representation of the configuration into a key, value and metadata."""

	values, metadata = internalToDictionary(internal)
	return processKeyData(values, metadata)


def internalToDictionary(
	internal: InternalFragmentList,
) -> typing.Tuple[typing.Dict[str, typing.Any], typing.Dict[str, Metadata]]:
	"""Convert the internal representation of the configuration into a dictionary."""

	output: typing.Dict[str, typing.Any] = {}
	outputMetadata: typing.Dict[str, Metadata] = {}
	for keyStr, value, metadata, source in internal:
		data = makeDictionary(keyStr, value)
		updateDeep(output, data, UpdatePolicy.override)
		if keyStr:
			outputMetadata[keyStr] = metadata
	return output, outputMetadata


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
	values = processKeyData(config, {})

	for key in args.keys:
		assert key in values, f"Key: '{key}' is not valid within the configuration."
		print(values[key].value)

	sys.exit(0)

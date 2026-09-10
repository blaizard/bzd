import argparse
import pathlib
import json
import re
import typing
import sys
import dataclasses

from bzd.utils.dict import updateDeep, UpdatePolicy

Metadata = typing.List[str]
InternalFragment = typing.Tuple[typing.Optional[str], typing.Any, Metadata, str]
InternalFragmentList = typing.List[InternalFragment]

# Escape character used within flat key strings to represent a literal '.' or '\'.
_ESCAPE_CHAR = "\\"


def _splitKey(keyStr: str) -> typing.List[str]:
	"""Split a flat key into its elements, unescaping escaped dots and backslashes."""

	# Protect escaped characters from the split, so only unescaped dots separate the elements.
	protected = re.sub(r"\\.", lambda m: m.group(0).replace(".", "\x00"), keyStr)
	return [re.sub(r"\\(.)", r"\1", element.replace("\x00", ".")) for element in protected.split(".")]


def _escapeKeyElement(key: str) -> str:
	"""Escape a key element so that it can be safely joined into a flat key string."""

	return key.replace(_ESCAPE_CHAR, _ESCAPE_CHAR * 2).replace(".", _ESCAPE_CHAR + ".")


@dataclasses.dataclass
class Data:
	# The value of the configuration key.
	value: typing.Any
	# The metadata associated with the configuration key.
	metadata: Metadata
	# The key of the configuration key (shallow).
	key: str


@dataclasses.dataclass
class KeyData:
	# Flat representation of the key/data.
	flat: typing.Dict[str, Data]
	# Nested representation of the key/data.
	nested: typing.Dict[str, Data]


def processKeyData(
	data: typing.Dict[str, typing.Any],
	metadata: typing.Dict[str, typing.Any],
) -> KeyData:
	"""Gather all the key/values."""

	valuesFlat: typing.Dict[str, Data] = {}

	def recursive(
		currentData: typing.Dict[str, typing.Any],
		currentRoot: typing.Optional[str],
	) -> typing.Dict[str, Data]:
		values = {}
		for key, value in currentData.items():
			keyStr = _escapeKeyElement(key) if currentRoot is None else f"{currentRoot}.{_escapeKeyElement(key)}"
			if isinstance(value, dict):
				value = recursive(value, keyStr)
			dataWithMetadata = Data(value=value, metadata=metadata.get(keyStr, []), key=key)
			values[keyStr] = dataWithMetadata
			valuesFlat[keyStr] = dataWithMetadata
		return values

	values = recursive(data, None)
	return KeyData(
		flat=valuesFlat,
		nested=values,
	)


def internalToKeyData(
	internal: InternalFragmentList,
) -> KeyData:
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
	[*keys, key] = _splitKey(keyStr)
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
		assert key in values.flat, f"Key: '{key}' is not valid within the configuration."
		print(values.flat[key].value)

	sys.exit(0)

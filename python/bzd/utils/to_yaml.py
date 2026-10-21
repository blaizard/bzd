import typing
import json

Yaml = typing.Union[typing.Dict[str, typing.Any], typing.List[typing.Any]]


def _valueToYaml(value: typing.Any) -> str:
	"""Format a simple Python value as a YAML scalar.

	Strings are JSON-encoded, which is valid YAML.
	"""
	if value is None:
		return "null"
	if isinstance(value, bool):
		return "true" if value else "false"
	if isinstance(value, (int, float)):
		return str(value)
	if isinstance(value, str):
		return json.dumps(value)

	raise Exception(f"Unsupported type: '{value}'")


def _toYaml(data: Yaml, indent: int) -> typing.List[str]:
	"""Recursively convert a JSON-like data structure into YAML lines.

	Args:
	        data: The dict or list to convert.
	        indent: The current indentation level.

	Returns:
	        A list of strings, where each string is a line of YAML.
	"""

	lines: typing.List[str] = []
	indentStr = " " * (indent * 2)

	if isinstance(data, dict):
		if not data:
			lines.append(f"{indentStr}{{}}")
			return lines

		for key, value in data.items():
			keyStr = f"{indentStr}{key}:"

			if isinstance(value, (dict, list)):
				lines.append(keyStr)
				lines.extend(_toYaml(value, indent + 1))
			else:
				lines.append(f"{keyStr} {_valueToYaml(value)}")

	elif isinstance(data, list):
		if not data:
			lines.append(f"{indentStr}[]")
			return lines

		for item in data:
			if isinstance(item, (dict, list)):
				lines.append(f"{indentStr}-")
				lines.extend(_toYaml(item, indent + 1))
			else:
				lines.append(f"{indentStr}- {_valueToYaml(item)}")

	return lines


def toYaml(data: Yaml) -> str:
	"""Convert a data structure to a YAML string."""

	yamlLines = _toYaml(data, indent=0)
	return "\n".join(yamlLines)

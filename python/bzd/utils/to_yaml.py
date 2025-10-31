import typing
import json

Yaml = typing.Union[typing.Dict[str, typing.Any], typing.List[typing.Any]]


def _valueToYaml(value: typing.Any) -> str:
	"""Formats simple Python types into YAML-compatible strings.
	Handles None, booleans, numbers, and basic strings.
	Adds quotes to strings that look like other types or contain special characters.
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
	"""Recursively converts a Python data structure (from JSON) into a list of YAML lines.
	
	Args:
		data: The Python data (dict, list, simple type) to convert.
		indent (int): The current indentation level.

	Returns:
		list: A list of strings, where each string is a line of YAML.
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
	"""Main function to convert a data structure to a YAML string."""

	yamlLines = _toYaml(data, indent=0)
	return "\n".join(yamlLines)

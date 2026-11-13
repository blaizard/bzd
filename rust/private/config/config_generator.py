import argparse
import json
import pathlib
import typing

from bzd.utils.runfiles import pathFromRLocation
from config.reader import internalToKeyData


def toRustString(value: str) -> str:
	"""Convert a string into a Rust string literal."""

	result = ['"']
	for character in value:
		if character == "\\":
			result.append("\\\\")
		elif character == '"':
			result.append('\\"')
		elif character == "\n":
			result.append("\\n")
		elif character == "\r":
			result.append("\\r")
		elif character == "\t":
			result.append("\\t")
		elif ord(character) < 0x20:
			result.append("\\u{{{:x}}}".format(ord(character)))
		else:
			result.append(character)
	result.append('"')
	return "".join(result)


def toRustByteArray(content: bytes) -> str:
	"""Convert bytes into a Rust static array literal."""

	lines = ["&["]
	for offset in range(0, len(content), 16):
		chunk = content[offset : offset + 16]
		lines.append("\t" + ", ".join("0x{:02X}".format(byte) for byte in chunk) + ",")
	lines.append("]")
	return "\n".join(lines)


def toRustLiteral(value: typing.Any, typeName: str) -> str:
	"""Convert a configuration value into a Rust literal."""

	if typeName == "str":
		return toRustString(str(value))
	if typeName == "bool":
		return "true" if value is True or str(value).lower() == "true" else "false"
	if typeName in ("usize", "i64"):
		assert typeName != "usize" or int(value) >= 0, f"Expected a non-negative integer, got {value!r}"
		return str(int(value))
	if typeName == "f64":
		return repr(float(value))
	assert False, f"Unsupported type '{typeName}'"


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Generate Rust configuration bindings.")
	parser.add_argument("--output", type=pathlib.Path, required=True)
	parser.add_argument("--internal", type=pathlib.Path, required=True)
	parser.add_argument("--expose", type=json.loads, required=True)
	parser.add_argument("--types", type=json.loads, required=True)
	args = parser.parse_args()

	assert set(args.expose) == set(args.types), "`expose` and `types` must have the same keys."

	flat = internalToKeyData(json.loads(args.internal.read_text())).flat
	lines = ["// This file was auto-generated.", "", "#![no_std]", ""]
	for key, name in args.expose.items():
		assert key in flat, f"Key '{key}' not found in configuration."
		assert "secret" not in flat[key].metadata, f"Secret configuration values are not supported (key '{key}')."

		value = flat[key].value
		typeName = args.types[key]
		if "embed" in flat[key].metadata:
			assert typeName in ("bytes", "str"), f"Embedded key '{key}' must be 'bytes' or 'str'."
			path = pathFromRLocation(value)
			if typeName == "bytes":
				lines.append(f"pub static {name}: &[u8] = {toRustByteArray(path.read_bytes())};")
			else:
				lines.append(f"pub const {name}: &str = {toRustString(path.read_text())};")
		else:
			assert typeName != "bytes", f"Key '{key}' of type 'bytes' must be embedded through srcs_at."
			lines.append(f"pub const {name}: {typeName} = {toRustLiteral(value, typeName)};")

	args.output.write_text("\n".join(lines) + "\n")

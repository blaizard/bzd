import argparse
import pathlib
import typing
import json
import typing
import sys

from bzd.utils.dict import updateDeep, UpdatePolicy


def fatal(message: str) -> None:
	print(message, file=sys.stderr)
	sys.exit(1)


class Config:

	def __init__(self, data: typing.Dict[str, typing.Any]) -> None:
		self.data = data
		self.keys = Config._processKeys(data)

	@staticmethod
	def _processKeys(data: typing.Dict[str, typing.Any]) -> typing.Set[str]:
		"""Gather all the keys."""

		keys = set()
		for key, value in data.items():
			if isinstance(value, dict):
				for nestedKeys in Config._processKeys(value):
					keys.add(f"{key}.{nestedKeys}")
			else:
				keys.add(key)
		return keys

	def setValue(self, key: str, value: typing.Any) -> None:
		"""Set a specific value in the configuration."""

		if key not in self.keys:
			self.fatal(f"The key '{key}' manually set is not present in the base configuration.")

		segments = key.split(".")
		current = self.data
		for segment in segments[:-1]:
			current = current[segment]
		current[segments[-1]] = value

	def update(self, data: typing.Dict[str, typing.Any]) -> None:
		"""Update the current configuration with another dictionary."""

		try:
			updateDeep(self.data, data, policy=UpdatePolicy.raiseOnNonConflict)
		except KeyError as e:
			self.fatal(f"The key {e} from '{f}' is not present in the base configuration.")

	def fatal(self, message: str) -> None:
		"""Error message."""

		print(message, file=sys.stderr)
		print("Available configuration keys are:", file=sys.stderr)
		for key in self.keys:
			print(f"  - {key}", file=sys.stderr)
		sys.exit(1)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Merge multiple JSON files together.")
	parser.add_argument("--output", default=None, type=pathlib.Path, help="The output path of the JSON file.")
	parser.add_argument("--fail-on-conflict",
	                    dest="failOnConflict",
	                    action="store_true",
	                    help="Fail if there is a conflict while merging files with the same keys.")
	parser.add_argument("--file",
	                    dest="files",
	                    default=[],
	                    action="append",
	                    type=pathlib.Path,
	                    help="file to be used for updating the config.")
	parser.add_argument("--set",
	                    dest="sets",
	                    default=[],
	                    action="append",
	                    type=str,
	                    help="Key/value pair to be used for updating the config, uses the format key=value.")

	parser.add_argument(
	    "inputs",
	    nargs="*",
	    type=pathlib.Path,
	    help="JSON input files to be merged.",
	)

	args = parser.parse_args()

	# Compute the unmodified output.
	output = {}
	for f in args.inputs:
		data = json.loads(f.read_text())
		try:
			updateDeep(output,
			           data,
			           policy=UpdatePolicy.raiseOnConflict if args.failOnConflict else UpdatePolicy.override)
		except KeyError as e:
			fatal(f"The key {e} from '{f}' was already defined by another base configuration.")

	config = Config(output)

	# Apply the files.
	for f in args.files:
		data = json.loads(f.read_text())
		config.update(data)

	# Apply the key value pairs.
	processedKeys = set()
	for keyValue in args.sets:
		key, value = keyValue.strip().split("=", 1)
		if key in processedKeys:
			fatal(f"The key '{key}' is manually set twice.")
		processedKeys.add(key)
		config.setValue(key, value)

	outputJson = json.dumps(output, indent=4)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

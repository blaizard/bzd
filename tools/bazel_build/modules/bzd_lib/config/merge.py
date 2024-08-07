import argparse
import pathlib
import typing
import json
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
			keys.add(key)
			if isinstance(value, dict):
				for nestedKeys in Config._processKeys(value):
					keys.add(f"{key}.{nestedKeys}")
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

	def fatal(self, message: str) -> None:
		"""Error message."""

		print(message, file=sys.stderr)
		print("Available configuration keys are:", file=sys.stderr)
		for key in sorted(self.keys):
			print(f"  - {key}", file=sys.stderr)
		sys.exit(1)


class ConfigValues:

	def __init__(self) -> None:
		self.data: typing.Dict[str, typing.Any] = {}

	def add(self, key: str, value: typing.Dict[str, typing.Any]) -> None:
		if key in self.data:
			fatal(f"The key '{key}' is manually set twice.")

		# Check there are conflict between parent/child keys. For example, setting `kvs.hello`` and `kvs.hello.world`.
		for existingKey in self.data.keys():
			if f"{key}." in existingKey:
				fatal(f"Cannot set a key '{key}' and a nested key '{existingKey}' at the same time.")
			if f"{existingKey}." in key:
				fatal(f"Cannot set a key '{existingKey}' and a nested key '{key}' at the same time.")

		self.data[key] = value

	def apply(self, config: Config) -> None:
		for key, value in self.data.items():
			config.setValue(key, value)


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
	parser.add_argument("--workspace-status-file",
	                    dest="workspaceStatusFiles",
	                    default=[],
	                    action="append",
	                    type=pathlib.Path,
	                    help="Key/value pair from a bazel workspace status.")
	parser.add_argument("--workspace-status-key",
	                    dest="workspaceStatusKeys",
	                    default=[],
	                    action="append",
	                    type=str,
	                    help="Keys to add to the configuration.")

	parser.add_argument(
	    "inputs",
	    nargs="*",
	    type=pathlib.Path,
	    help="JSON input files to be merged.",
	)

	args = parser.parse_args()

	# Compute the workspace status files.
	workspaceStatus = {}
	for workspaceStatusFile in args.workspaceStatusFiles:
		for line in workspaceStatusFile.read_text().split("\n"):
			try:
				[key, value] = line.split(" ", 1)
				workspaceStatus[key.strip()] = value.strip()
			except ValueError:
				pass
	# Keep only specific keys.
	output = {key: workspaceStatus[key] for key in args.workspaceStatusKeys}

	# Compute the unmodified output.
	for f in args.inputs:
		data = json.loads(f.read_text())
		try:
			updateDeep(output,
			           data,
			           policy=UpdatePolicy.raiseOnConflict if args.failOnConflict else UpdatePolicy.override)
		except KeyError as e:
			fatal(f"The key {e} from '{f}' was already defined by another base configuration.")

	config = Config(output)
	values = ConfigValues()

	# Apply the files.
	for f in args.files:
		data = json.loads(f.read_text())
		for key, value in data.items():
			values.add(key, value)

	# Apply the key value pairs.
	for keyValue in args.sets:
		key, value = keyValue.strip().split("=", 1)
		values.add(key, value)

	# Process the values
	values.apply(config)

	outputJson = json.dumps(output, indent=4)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

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

	def overrideValue(self, key: str, value: typing.Any, source: str) -> None:
		"""Set a specific value in the configuration."""

		try:
			assert key in self.keys, f"The key '{key}' is not present in the base configuration."

			segments = key.split(".")
			current = self.data
			for segment in segments[:-1]:
				current = current[segment]
			previousValue = current[segments[-1]]

			# If the override is a list, the original value MUST be a list.
			if isinstance(value, list):
				assert isinstance(
				    previousValue, list
				), f"Trying to assign a value of type 'list' to the key '{key}' that is not a list but a '{type(previousValue)}'."

			# If the original value is a list, adjust the override to be a list.
			if isinstance(previousValue, list):
				if not isinstance(value, list):
					value = [value]

			current[segments[-1]] = value

		except AssertionError as e:
			self.fatal(f"[{source}] {str(e)}")

	def override(self, *overrides: "ConfigOverride") -> None:
		"""Override multiple config."""

		# Merge all config together and check for conflicts
		data: typing.Dict[str, typing.Tuple[str, typing.Any]] = {}
		for override in overrides:
			for key, value in override.data.items():
				if key in data:
					fatal(f"The key '{key}' is manually set twice by '{data[key][0]}' and '{override.source}'.")

				# Check there are conflict between parent/child keys. For example, setting `kvs.hello`` and `kvs.hello.world`.
				for existingKey in data.keys():
					if f"{key}." in existingKey:
						fatal(
						    f"Cannot set a key '{key}' (from '{override.source}') and a nested key '{existingKey}' (from '{data[existingKey][0]}') at the same time."
						)
					if f"{existingKey}." in key:
						fatal(
						    f"Cannot set a key '{existingKey}' (from '{data[existingKey][0]}') and a nested key '{key}' (from '{override.source}') at the same time."
						)

				data[key] = (
				    override.source,
				    value,
				)

		# Set the values.
		for key, value in data.items():
			config.overrideValue(key, value[1], value[0])

	def fatal(self, message: str) -> None:
		"""Error message."""

		content: typing.List[str] = []
		content.append(message)
		content.append("Available configuration keys are:")
		for key in sorted(self.keys):
			content.append(f"  - {key}")
		fatal("\n".join(content))


class ConfigOverride:

	def __init__(self, source: str) -> None:
		self.source = source
		self.data: typing.Dict[str, typing.Any] = {}

	def add(self, key: str, value: typing.Dict[str, typing.Any]) -> None:
		# Create a list if the data is set multiple times.
		if key in self.data:
			if not isinstance(self.data[key], list):
				self.data[key] = [self.data[key]]
			self.data[key].append(value)

		# Otherwise just assign it.
		else:
			self.data[key] = value

	def apply(self, config: Config) -> None:
		for key, value in self.data.items():
			config.overrideValue(key, value, self.source)

	def fatal(self, message: str) -> None:
		"""Error message."""

		fatal(f"{self.source} {message}")


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
	overrides: typing.List[ConfigOverride] = []

	# Apply the files.
	for f in args.files:
		override = ConfigOverride(str(f))
		data = json.loads(f.read_text())
		for key, value in data.items():
			override.add(key, value)
		overrides.append(override)

	# Apply the key value pairs.
	override = ConfigOverride("command line")
	for keyValue in args.sets:
		key, value = keyValue.strip().split("=", 1)
		override.add(key, value)
	overrides.append(override)

	# Process the values
	config.override(*overrides)

	outputJson = json.dumps(output, indent=4)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

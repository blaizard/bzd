import argparse
import pathlib
import typing
import json
import sys
import yaml

from bzd.utils.dict import updateDeep, UpdatePolicy
from config.reader import (
	InternalFragmentList,
	InternalFragment,
	makeDictionary,
	internalToDictionary,
)


def fatal(message: str) -> None:
	print(message, file=sys.stderr)
	sys.exit(1)


class Config:
	def __init__(self) -> None:
		self.data: InternalFragmentList = []
		self.dataAsDict: typing.Dict[str, typing.Any] = {}

	def addKey(
		self,
		key: typing.Optional[str],
		value: typing.Any,
		source: str,
		failOnConflict: bool,
	) -> None:
		data = makeDictionary(key, value)
		self._addData(data, source, failOnConflict)
		self.data.append((key, value, source))

	def addDict(self, data: typing.Dict[str, typing.Any], source: str, failOnConflict: bool) -> None:
		self._addData(data, source, failOnConflict)
		self.data.append((None, data, source))

	def useBase(self, base: "Config") -> None:
		self.data = [*base.data, *self.data]
		self.dataAsDict = internalToDictionary(self.data)

	def _addData(self, data: typing.Dict[str, typing.Any], source: str, failOnConflict: bool) -> None:

		def policy(originalValue: typing.Any, newValue: typing.Any, keys: typing.List[str]) -> None:
			if originalValue is None or newValue is None:
				return
			if isinstance(originalValue, list) and not isinstance(newValue, list):
				fatal(f"The override of a list should be a list, see key '{'.'.join(keys)}' from '{source}'.")
			return

		try:
			updateDeep(
				self.dataAsDict,
				data,
				policy=UpdatePolicy.raiseOnConflict if failOnConflict else policy,
			)
		except KeyError as e:
			fatal(f"The key {e} from '{source}' was already defined by another base configuration.")


def dataFromJson(path: pathlib.Path) -> typing.Any:
	"""Load the content of a JSON file from its path."""

	def resolvePath(ref: str) -> pathlib.Path:
		refPath = pathlib.Path(ref)
		if not refPath.is_file():
			refPath = path.parent / refPath
			if not refPath.is_file():
				fatal(f"Reference path '{ref}' not found from '{str(path)}'.")
		return refPath

	def visitJson(data: typing.Any) -> typing.Any:
		if isinstance(data, dict):
			# Add support for references, the value of a reference can be either a single string or an array of strings.
			refs = [data["$ref"]] if "$ref" in data else []
			refs.extend(data.get("$refs", []))
			data.pop("$ref", None)
			data.pop("$refs", None)
			for ref in refs:
				refPath = resolvePath(ref)
				updateDeep(
					data,
					dataFromSrc(refPath),
					policy=UpdatePolicy.raiseOnConflict,
				)
			return {key: visitJson(value) for key, value in data.items()}
		elif isinstance(data, list):
			return [visitJson(item) for item in data]
		return data

	data = json.loads(path.read_text())
	return visitJson(data)


def dataFromSrc(path: pathlib.Path) -> typing.Optional[typing.Any]:
	"""Load the content of a source file from its path."""

	extension = path.suffix.lower()
	if extension == ".json":
		return dataFromJson(path)

	elif extension in (
		".yaml",
		".yml",
	):
		with open(path, "r") as f:
			return yaml.load(f, Loader=yaml.SafeLoader)  # type: ignore
	return None


def dataFromPath(path: pathlib.Path) -> typing.Iterator[InternalFragment]:
	"""Load the content of a file from its path."""

	extension = path.suffix.lower()
	maybeContent = dataFromSrc(path)
	if maybeContent is not None:
		yield (None, maybeContent, str(path))

	elif extension == ".internal":
		fragments: InternalFragmentList = json.loads(path.read_text())
		for key, value, source in fragments:
			yield (key, value, source)

	else:
		fatal(f"File extension '{extension}' not supported: {str(path)}.")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Merge multiple JSON files together.")
	parser.add_argument(
		"--output",
		default=None,
		type=pathlib.Path,
		help="The output path of the JSON file.",
	)
	parser.add_argument("--base", type=pathlib.Path, help="file to be used as a base.")
	parser.add_argument(
		"--override-set",
		dest="overrideSets",
		default=[],
		action="append",
		type=str,
		help="Key/value pair to be used for updating the config, uses the format key=value.",
	)
	parser.add_argument(
		"--workspace-status-file",
		dest="workspaceStatusFiles",
		default=[],
		action="append",
		type=pathlib.Path,
		help="Key/value pair from a bazel workspace status.",
	)
	parser.add_argument(
		"--workspace-status-key",
		dest="workspaceStatusKeys",
		default=[],
		action="append",
		type=str,
		help="Keys to add to the configuration.",
	)
	parser.add_argument(
		"--src",
		dest="srcs",
		default=[],
		action="append",
		type=pathlib.Path,
		help="JSON input files to be merged.",
	)
	parser.add_argument(
		"--value-at",
		dest="values_at",
		default=[],
		action="append",
		nargs=2,
		metavar=("<KEY>", "<VALUE>"),
		help="Value to be merged at a given key.",
	)
	parser.add_argument(
		"--src-at",
		dest="srcs_at",
		default=[],
		action="append",
		nargs=2,
		metavar=("<KEY>", "<FILE>"),
		help="JSON input files to be merged at a given key.",
	)

	args = parser.parse_args()

	# Keep only specific keys.
	output = Config()

	# Create the output, cannot override the same key twice.

	# - From workspace status files.
	workspaceStatusKeysUsed: typing.Set[str] = set()
	for workspaceStatusFile in args.workspaceStatusFiles:
		workspaceStatus = {}
		for line in workspaceStatusFile.read_text().split("\n"):
			try:
				[key, value] = line.split(" ", 1)
				workspaceStatus[key.strip()] = value.strip()
			except ValueError:
				pass
		filteredWorkspaceStatus = {key: workspaceStatus[key] for key in args.workspaceStatusKeys if key in workspaceStatus}
		output.addDict(
			filteredWorkspaceStatus,
			source=str(workspaceStatusFile),
			failOnConflict=True,
		)
		workspaceStatusKeysUsed.update(filteredWorkspaceStatus.keys())
	assert len(workspaceStatusKeysUsed) == len(args.workspaceStatusKeys), "Some workspace status keys were not found."

	# - From files.
	for f in args.srcs:
		for _, value, source in dataFromPath(f):
			output.addDict(value, source=source, failOnConflict=True)

	# - From files at a specified key.
	for keyStr, f in args.srcs_at:
		for _, value, source in dataFromPath(pathlib.Path(f)):
			output.addKey(keyStr, value, source=source, failOnConflict=True)

	# - From values at a specified key.
	for keyStr, value in args.values_at:
		output.addKey(keyStr, value, source="values_at", failOnConflict=True)

	# Base and overrides, can override existing keys.

	# Apply the files.
	if args.base:
		base = Config()
		for key, value, source in dataFromPath(args.base):
			base.addKey(key, value, source=source, failOnConflict=False)
		output.useBase(base)

	# Apply the key value pairs.
	for keyValue in args.overrideSets:
		key, value = keyValue.strip().split("=", 1)
		if value.startswith("[") and value.endswith("]"):
			value = [value[1:-1]]
		output.addKey(key, value, source="command line", failOnConflict=False)

	outputJson = json.dumps(output.data)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

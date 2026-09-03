import argparse
import pathlib
import typing
import json
import sys
import yaml

from bzd.utils.dict import updateDeep, UpdatePolicy
from config.reader import (
	Metadata,
	InternalFragmentList,
	InternalFragment,
	makeDictionary,
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
		metadata: Metadata,
		source: str,
		policy: UpdatePolicy,
	) -> None:
		data = makeDictionary(key, value)
		self._verifyData(data, source, policy)
		self.data.append((key, value, metadata, source))

	def addDict(self, data: typing.Dict[str, typing.Any], metadata: Metadata, source: str, policy: UpdatePolicy) -> None:
		self._verifyData(data, source, policy)
		self.data.append((None, data, metadata, source))

	def _verifyData(self, data: typing.Dict[str, typing.Any], source: str, policy: UpdatePolicy) -> None:

		def policyOverride(originalValue: typing.Any, newValue: typing.Any, keys: typing.List[str]) -> None:
			if originalValue is None or newValue is None:
				return
			if isinstance(originalValue, list) and not isinstance(newValue, list):
				fatal(f"The override of a list should be a list, see key '{'.'.join(keys)}' from '{source}'.")
			return

		try:
			updateDeep(
				self.dataAsDict,
				data,
				policy=policyOverride if policy == UpdatePolicy.override else policy,
			)
		except KeyError as e:
			if policy == UpdatePolicy.raiseOnConflict:
				fatal(f"The key {e} from '{source}' was already defined by another base configuration.")
			elif policy == UpdatePolicy.raiseOnNonConflict:
				fatal(f"The key {e} from '{source}' is marked as 'override' but is not overwriting an existing key.")
			raise


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
				refData = dataFromSrc(refPath)
				assert refData is not None, f"Reference path '{ref}' from '{str(path)}' could not be loaded."
				updateDeep(
					data,
					refData,
					policy=UpdatePolicy.raiseOnConflict,
					extendLists=True,
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


def dataFromPath(path: pathlib.Path, mustBeDictionary: bool) -> typing.Iterator[InternalFragment]:
	"""Load the content of a file from its path."""

	extension = path.suffix.lower()
	maybeContent = dataFromSrc(path)
	if maybeContent is not None:
		yield (None, maybeContent, [], str(path))

	elif extension == ".internal":
		fragments: InternalFragmentList = json.loads(path.read_text())
		for key, value, metadata, source in fragments:
			yield (key, value, metadata, source)

	elif mustBeDictionary:
		fatal(f"File extension '{extension}' not supported: {str(path)}.")

	else:
		yield (None, path.read_text(), [], str(path))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Merge multiple JSON files together.")
	parser.add_argument(
		"--output",
		default=None,
		type=pathlib.Path,
		help="The output path of the JSON file.",
	)
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
		"--src-at",
		dest="srcs_at",
		default=[],
		action="append",
		help="JSON input files to be merged at a given key.",
	)

	parser.add_argument(
		"--value",
		dest="values",
		default=[],
		action="append",
		help="Value to be merged at a given key.",
	)

	args = parser.parse_args()

	# Create the output configuration.
	output = Config()

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
			metadata=[],
			source=str(workspaceStatusFile),
			policy=UpdatePolicy.raiseOnConflict,
		)
		workspaceStatusKeysUsed.update(filteredWorkspaceStatus.keys())
	assert len(workspaceStatusKeysUsed) == len(args.workspaceStatusKeys), "Some workspace status keys were not found."

	# - From files.
	for f in args.srcs:
		for key, value, metadata, source in dataFromPath(f, mustBeDictionary=True):
			if key is None:
				output.addDict(value, metadata=metadata, source=source, policy=UpdatePolicy.override)
			else:
				output.addKey(key, value, metadata=metadata, source=source, policy=UpdatePolicy.override)

	# - From files at a specified key.
	for value in args.srcs_at:
		keyStr, f, metadata = json.loads(value)
		for subKeyStr, value, subMetadata, source in dataFromPath(pathlib.Path(f), mustBeDictionary=False):
			output.addKey(
				f"{keyStr}.{subKeyStr}" if subKeyStr else keyStr,
				value,
				metadata=metadata + subMetadata,
				source=source,
				policy=UpdatePolicy.override,
			)

	# - From values at a specified key.
	for entry in args.values:
		keyStr, value, metadata = json.loads(entry)
		output.addKey(keyStr, value, metadata=metadata, source="values", policy=UpdatePolicy.override)

	# Apply the key value pairs from the command line.
	for keyValue in args.overrideSets:
		key, value = keyValue.strip().split("=", 1)
		if value.startswith("[") and value.endswith("]"):
			value = [value[1:-1]]
		output.addKey(key, value, metadata=[], source="command line", policy=UpdatePolicy.override)

	outputJson = json.dumps(output.data)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

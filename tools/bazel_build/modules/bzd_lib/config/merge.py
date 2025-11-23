import argparse
import pathlib
import typing
import json
import sys
import yaml

from bzd.utils.dict import updateDeep, UpdatePolicy
from config.reader import internalToDictionary, InternalFragmentList


def fatal(message: str) -> None:
	print(message, file=sys.stderr)
	sys.exit(1)


class Config:

	def __init__(self) -> None:
		self.data: InternalFragmentList = []
		self.dataAsDict: typing.Dict[str, typing.Any] = {}

	def addKey(self, key: str, value: typing.Any, source: str, failOnConflict: bool) -> None:
		data = makeDictionary(key, value)
		self._addData(data, source, failOnConflict)
		self.data.append((key, value, source))

	def addDict(self, data: typing.Dict[str, typing.Any], source: str, failOnConflict: bool) -> None:
		self._addData(data, source, failOnConflict)
		self.data.append((None, data, source))

	def _addData(self, data: typing.Dict[str, typing.Any], source: str, failOnConflict: bool) -> None:
		try:
			updateDeep(self.dataAsDict,
			           data,
			           policy=UpdatePolicy.raiseOnConflict if failOnConflict else UpdatePolicy.override)
		except KeyError as e:
			fatal(f"The key {e} from '{source}' was already defined by another base configuration.")


def makeDictionary(keyStr: str, value: typing.Any) -> typing.Dict[str, typing.Any]:
	"""Make a dictionary from a key string."""

	root: typing.Dict[str, typing.Any] = {}
	data = root
	[*keys, key] = keyStr.split(".")
	for k in keys:
		data[k] = {}
		data = data[k]
	data[key] = value

	return root


def updateOutput(output: typing.Dict[str, typing.Any], data: typing.Dict[str, typing.Any],
                 failOnConflict: bool) -> None:
	try:
		updateDeep(output, data, policy=UpdatePolicy.raiseOnConflict if failOnConflict else UpdatePolicy.override)
	except KeyError as e:
		fatal(f"The key {e} from '{f}' was already defined by another base configuration.")


def dataFromPath(path: pathlib.Path) -> typing.Dict[str, typing.Any]:
	"""Load the content of a file from its path."""

	extension = path.suffix.lower()
	if extension == ".json":
		content = json.loads(path.read_text())
	elif extension in (
	    ".yaml",
	    ".yml",
	):
		with open(path, "r") as f:
			content = yaml.load(f, Loader=yaml.SafeLoader)  # type: ignore
	else:
		fatal(f"File extension '{extension}' not supported: {str(path)}.")

	assert isinstance(content, dict), f"The content of the data at '{path}' must be a dictionary."
	return content


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Merge multiple JSON files together.")
	parser.add_argument("--output", default=None, type=pathlib.Path, help="The output path of the JSON file.")
	parser.add_argument("--fail-on-conflict",
	                    dest="failOnConflict",
	                    action="store_true",
	                    help="Fail if there is a conflict while merging files with the same keys.")
	parser.add_argument("--override-file",
	                    dest="overrideFiles",
	                    default=[],
	                    action="append",
	                    type=pathlib.Path,
	                    help="file to be used for updating the config.")
	parser.add_argument("--override-set",
	                    dest="overrideSets",
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
	parser.add_argument("--src",
	                    dest="srcs",
	                    default=[],
	                    action="append",
	                    type=pathlib.Path,
	                    help="JSON input files to be merged.")
	parser.add_argument("--value-at",
	                    dest="values_at",
	                    default=[],
	                    action="append",
	                    nargs=2,
	                    metavar=("<KEY>", "<VALUE>"),
	                    help="Value to be merged at a given key.")
	parser.add_argument("--src-at",
	                    dest="srcs_at",
	                    default=[],
	                    action="append",
	                    nargs=2,
	                    metavar=("<KEY>", "<FILE>"),
	                    help="JSON input files to be merged at a given key.")

	args = parser.parse_args()

	# Keep only specific keys.
	output = Config()

	# Create the output, cannot override the same key twice.

	# - From workspace status files.
	workspaceStatusKeysUsed = set()
	for workspaceStatusFile in args.workspaceStatusFiles:
		workspaceStatus = {}
		for line in workspaceStatusFile.read_text().split("\n"):
			try:
				[key, value] = line.split(" ", 1)
				workspaceStatus[key.strip()] = value.strip()
			except ValueError:
				pass
		filteredWorkspaceStatus = {
		    key: workspaceStatus[key]
		    for key in args.workspaceStatusKeys if key in workspaceStatus
		}
		output.addDict(filteredWorkspaceStatus, source=str(workspaceStatusFile), failOnConflict=True)
		workspaceStatusKeysUsed.update(filteredWorkspaceStatus.keys())
	assert len(workspaceStatusKeysUsed) == len(args.workspaceStatusKeys), "Some workspace status keys were not found."

	# - From files.
	for f in args.srcs:
		data = dataFromPath(f)
		output.addDict(data, source=str(f), failOnConflict=True)

	# - From files at a specified key.
	for keyStr, f in args.srcs_at:
		value = dataFromPath(pathlib.Path(f))
		output.addKey(keyStr, value, source=str(f), failOnConflict=True)

	# - From values at a specified key.
	for keyStr, value in args.values_at:
		output.addKey(keyStr, value, source="values_at", failOnConflict=True)

	# Overrides, can override existing keys.

	# Apply the files.
	for f in args.overrideFiles:
		fragments: InternalFragmentList = json.loads(f.read_text())
		for key, value, source in fragments:
			output.addKey(key, value, source=source, failOnConflict=False)

	# Apply the key value pairs.
	for keyValue in args.overrideSets:
		key, value = keyValue.strip().split("=", 1)
		output.addKey(key, value, source="command line", failOnConflict=False)

	outputJson = json.dumps(output.data)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

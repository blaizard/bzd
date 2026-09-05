import argparse
import json
import pathlib
import typing
import sys

from bzd_sanitizer.context import Context
from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary, localCommand, StreamCallback

Secrets = typing.Dict[pathlib.Path, typing.Dict[str, typing.Any]]


def workload(
	args: typing.Tuple[pathlib.Path, pathlib.Path, bool, pathlib.Path, Secrets],
	stdout: typing.TextIO,
) -> bool:
	workspace, path, check, secret, secrets = args
	attributes = secrets[path]

	params = ["--file", str(workspace / path)]
	for recipient in attributes.get("recipients", []):
		params += ["--recipient", recipient]
	params += ["check"] if check else ["--output", str(workspace / path), "encrypt"]

	result = localBazelBinary(
		secret.as_posix(),
		args=params,
		ignoreFailure=True,
		stdout=stdout,
		stderr=stdout,
	)

	return result.isSuccess()


def listAllSecrets(workspace: pathlib.Path) -> Secrets:
	secrets: typing.Dict[str, pathlib.Path] = {}
	rules: typing.Dict[str, typing.Dict[str, typing.Any]] = {}

	def fetchFileNames(line: str) -> None:
		data = json.loads(line)
		dataType = data["type"]

		if dataType == "SOURCE_FILE":
			target = data["sourceFile"]["name"]
			location = data["sourceFile"]["location"]
			path = pathlib.Path(location.rsplit(":", 2)[0])
			secrets[target] = path.relative_to(context.workspace)

		elif dataType == "RULE":
			attributes = {}
			for attribute in data["rule"]["attribute"]:
				name = attribute["name"]
				print(attribute)
				if name == "src" and attribute["explicitlySpecified"]:
					attributes[name] = attribute["stringValue"]
				elif name == "recipients" and attribute["explicitlySpecified"]:
					attributes[name] = attribute["stringListValue"]

			if "src" in attributes:
				target = attributes["src"]
				if target in rules:
					print(f"The secret file '{target}' must be owned by only one rule.", flush=True)
					sys.exit(1)
				rules[target] = attributes

		else:
			raise KeyError(f"Unsupported type '{dataType}'")

	localCommand(
		[
			"bazel",
			"query",
			'labels(src, kind("bzd_secret rule", //...)) + kind("bzd_secret rule", //...)',
			"--output=streamed_jsonproto",
			"--noimplicit_deps",
			"--nohost_deps",
		],
		cwd=workspace,
		stdout=StreamCallback(callback=fetchFileNames, mode=StreamCallback.Mode.LINE),  # type: ignore
	)

	result = {}
	for target, path in secrets.items():
		if target not in rules:
			print(f"Could not identify rule for secret '{target}'.")
		result[path] = rules[target]

	return result


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Update and check secrets.")
	parser.add_argument("--secret", type=pathlib.Path, help="The path of the secret binary.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	context = Context.fromFile(args.context)

	result = listAllSecrets(workspace=context.workspace)

	worker(
		args.context,
		workload,
		args=(
			args.secret,
			result,
		),
		includeFiles=result.keys(),
	)

import argparse
import json
import pathlib
import typing

from bzd_sanitizer.context import Context
from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary, localCommand, StreamCallback


def workload(
	args: typing.Tuple[pathlib.Path, pathlib.Path, bool, pathlib.Path],
	stdout: typing.TextIO,
) -> bool:
	workspace, path, check, secret = args

	params = ["--file", str(workspace / path)]
	if check:
		params += ["check"]
	else:
		params += ["--output", str(workspace / path), "encrypt"]

	result = localBazelBinary(
		secret.as_posix(),
		args=params,
		ignoreFailure=True,
		stdout=stdout,
		stderr=stdout,
	)

	return result.isSuccess()


def listAllSecretFiles(workspace: pathlib.Path) -> typing.Set[pathlib.Path]:
	secrets = set()

	def fetchFileNames(line: str) -> None:
		data = json.loads(line)
		location = data["sourceFile"]["location"]
		path = pathlib.Path(location.rsplit(":", 2)[0])
		secrets.add(path.relative_to(context.workspace))

	localCommand(
		[
			"bazel",
			"query",
			'labels(src, kind("bzd_secret rule", //...))',
			"--output=streamed_jsonproto",
			"--noimplicit_deps",
			"--nohost_deps",
		],
		cwd=workspace,
		stdout=StreamCallback(callback=fetchFileNames, mode=StreamCallback.Mode.LINE),  # type: ignore
	)

	return secrets


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Update and check secrets.")
	parser.add_argument("--secret", type=pathlib.Path, help="The path of the secret binary.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	context = Context.fromFile(args.context)

	secretFiles = listAllSecretFiles(workspace=context.workspace)

	worker(
		args.context,
		workload,
		args=(args.secret,),
		includeFiles=secretFiles,
	)

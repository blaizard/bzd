import argparse
import io
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, str, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, ruff, config = args

	params = ["format"]
	params += ["--check"] if check else []
	params += ["--config", config] if check else []
	resultFormat = localBazelBinary(
	    ruff,
	    args=params + [str(workspace / path)],
	    ignoreFailure=True,
	)
	if resultFormat.isFailure():
		stdout.write("This file is not formatted properly.\n")

	params = ["check", "--color=always"]
	params += [] if check else ["--fix"]
	params += ["--config", config] if check else []
	stdoutCheck = io.StringIO()
	resultCheck = localBazelBinary(
	    ruff,
	    args=params + [str(workspace / path)],
	    ignoreFailure=True,
	    stdout=stdoutCheck,
	    stderr=stdoutCheck,
	)
	if resultCheck.isFailure():
		stdout.write(stdoutCheck.getvalue())

	return resultFormat.isSuccess() and resultCheck.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Ruff.")
	parser.add_argument("--ruff", type=pathlib.Path, help="The path of the ruff binary.")
	parser.add_argument(
	    "--config",
	    type=pathlib.Path,
	    help="The path of the configuration file for ruff.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,
	    args=(
	        str(args.ruff),
	        str(args.config),
	    ),
	    endswith=[".py", ".pyi"],
	    excludeFile=".sanitizerignore",
	)

import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, buildifier = args

	if not check:
		result = localBazelBinary(
		    buildifier,
		    args=["-lint", "fix", "-warnings=all", str(workspace / path)],
		    ignoreFailure=True,
		    stdout=stdout,
		    stderr=stdout,
		)
		if result.isFailure():
			return False

	result = localBazelBinary(
	    buildifier,
	    args=["-lint", "warn", "-warnings=all", str(workspace / path)],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Buildifier.")
	parser.add_argument(
	    "-b",
	    "--buildifier",
	    type=pathlib.Path,
	    help="The path of the buildifier binary.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,  # type: ignore
	    args=[str(args.buildifier)],
	    endswith=[".bazel", ".bzl"],
	    include=["**BUILD", "**WORKSPACE"],
	)

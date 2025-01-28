import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, rustfmt = args

	params = []
	params += ["--check"] if check else []

	result = localBazelBinary(
	    rustfmt,
	    args=params + [str(workspace / path)],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Rustfmt.")
	parser.add_argument("--rustfmt", type=pathlib.Path, help="The path of the rustfmt binary.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,
	    args=(str(args.rustfmt), ),
	    endswith=[".rs"],
	)

import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[str, str, bool, str, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, black, config = args

	args = []
	args += ["--check"] if check else []
	args += ["--config", config] if check else []

	result = localBazelBinary(
	    black,
	    args=args + [workspace / path],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Black.")
	parser.add_argument("--black", type=pathlib.Path, help="The path of the black binary.")
	parser.add_argument(
	    "--config",
	    type=pathlib.Path,
	    help="The path of the configuration file for black.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,
	    args=[str(args.black), str(args.config)],
	    endswith=[".py", ".pyi"],
	)

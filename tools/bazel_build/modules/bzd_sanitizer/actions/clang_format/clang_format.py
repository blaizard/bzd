import argparse
import pathlib
import typing

from lib.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[str, str, bool, str, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, clang_format, config = args

	args = ["--sort-includes"]
	args += ["--dry-run"] if check else ["-i"]
	if config:
		args += [f"--style=file:{config}"]

	result = localBazelBinary(
	    clang_format,
	    args=args + [str(workspace / path)],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Clang-format.")
	parser.add_argument("--clang-format", type=pathlib.Path, help="The path of the clang-format binary.")
	parser.add_argument(
	    "--config",
	    type=str,
	    default="",
	    help="The path of the clang-format configuration file.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,
	    args=[str(args.clang_format), args.config],
	    endswith=[".c", ".cc", ".cpp", ".h", ".hh", ".hpp"],
	)

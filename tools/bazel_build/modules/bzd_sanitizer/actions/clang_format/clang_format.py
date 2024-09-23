import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, str, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, clang_format, config = args

	params = ["--sort-includes"]
	params += ["--dry-run"] if check else ["-i"]
	if config:
		params += [f"--style=file:{config}"]

	result = localBazelBinary(
	    clang_format,
	    args=params + [str(workspace / path)],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Clang-format for C/C++/Javascript.")
	parser.add_argument("--clang-format", type=pathlib.Path, help="The path of the clang-format binary.")
	parser.add_argument("--cc", action="store_true", help="Use clang format for C/C++ files.")
	parser.add_argument("--js", action="store_true", help="Use clang format for Javascript files.")
	parser.add_argument(
	    "--config",
	    type=str,
	    default="",
	    help="The path of the clang-format configuration file.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	extensions = []
	if args.cc:
		extensions += [".c", ".cc", ".cpp", ".h", ".hh", ".hpp"]
	if args.js:
		extensions += [".mjs", ".js", ".ts"]

	worker(
	    args.context,
	    workload,
	    args=(
	        str(args.clang_format),
	        args.config,
	    ),
	    endswith=extensions,
	)

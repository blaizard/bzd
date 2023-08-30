import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[str, str, bool, str, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, prettier, config = args

	args = []
	args += ["--check"] if check else ["--write"]
	args += ["--config", config] if config else []

	result = localBazelBinary(
	    prettier,
	    args=args + [workspace / path],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Prettier.")
	parser.add_argument("--prettier", type=pathlib.Path, help="The path of the prettier wrapper.")
	parser.add_argument("--config", type=str, default="", help="The path of the configuration for prettier.")
	parser.add_argument("--endswith", action="append", required=True, help="The extension to select files.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,
	    args=[str(args.prettier), str(args.config)],
	    endswith=args.endswith,
	)

import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, str, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, codespell, config = args

	params = []
	params += [] if check else ["--write-changes"]
	params += ["--config", config] if check else []

	result = localBazelBinary(
	    codespell,
	    args=params + [str(workspace / path)],
	    ignoreFailure=True,
	    stdout=stdout,
	    stderr=stdout,
	)

	return result.isSuccess()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Codespell.")
	parser.add_argument("--codespell", type=pathlib.Path, help="The path of the codespell binary.")
	parser.add_argument(
	    "--config",
	    type=pathlib.Path,
	    help="The path of the configuration file for codespell.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,
	    args=[str(args.codespell), str(args.config)],
	)

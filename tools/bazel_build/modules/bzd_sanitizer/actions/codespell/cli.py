import argparse
import pathlib
import typing
import re

from bzd_sanitizer.worker import chunkWorker, OutputWithPath
from bzd.utils.run import localBazelBinary


def stdoutParser(stdout: str, workspace: pathlib.Path) -> typing.Generator[OutputWithPath, None, None]:
	"""Extract errors from the stdout."""

	for line in stdout.split("\n"):
		m = re.match(r"([^:]+):", line)
		if m:
			yield OutputWithPath(path=pathlib.Path(m.group(1)), output=line)


def workload(args: typing.Tuple[pathlib.Path, typing.Sequence[pathlib.Path], bool, str, typing.Optional[pathlib.Path],
                                typing.Optional[pathlib.Path]], stdout: typing.TextIO) -> bool:
	workspace, paths, check, codespell, config, toml = args

	params = []
	params += [] if check else ["--write-changes"]
	params += ["--config", str(config)] if config else []
	params += ["--toml", str(toml)] if toml else []

	result = localBazelBinary(
	    codespell,
	    args=params + [str(workspace / path) for path in paths],
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
	parser.add_argument(
	    "--toml",
	    type=pathlib.Path,
	    help="The path of the pyproject.toml file for codespell.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	chunkWorker(
	    args.context,
	    workload,
	    stdoutParser=stdoutParser,
	    args=(
	        str(args.codespell),
	        args.config,
	        args.toml,
	    ),
	    excludeFile=".codespellignore",
	)

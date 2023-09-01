import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from yapf.yapflib.yapf_api import FormatFile


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, str], stdout: typing.TextIO) -> bool:
	workspace, path, check, config = args

	_, _, has_change = FormatFile(workspace / path,
	                              in_place=not check,
	                              style_config=config if config else None,
	                              logger=stdout)

	if check and has_change:
		stdout.write("This file requires formatting.\n")
		return False

	return True


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Yapf.")
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
	    args=[str(args.config)],
	    endswith=[".py", ".pyi"],
	)

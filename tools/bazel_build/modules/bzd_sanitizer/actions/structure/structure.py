import argparse
import pathlib
import typing
import re

from bzd_sanitizer.worker import worker


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool, typing.Pattern[str]], stdout: typing.TextIO) -> bool:
	workspace, path, check, regexpr = args

	if not regexpr.match(str(path)):
		stdout.write(f"This path does not match the naming convention '{regexpr}'.\n")
		return False
	return True


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Structure checker.")
	parser.add_argument(
	    "--regexpr",
	    default="^[a-z0-9_/.]+(\.S)?$",
	    help="Regular expression to be used for the matching.",
	)
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(
	    args.context,
	    workload,  # type: ignore
	    args=[re.compile(args.regexpr)],
	    excludeFile=".structureignore",
	)

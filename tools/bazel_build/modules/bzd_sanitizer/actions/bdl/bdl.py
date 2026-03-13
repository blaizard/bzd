import argparse
import pathlib
import typing

from bzd_sanitizer.worker import worker
from bdl.lib import main as bdlFormatter


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool], stdout: typing.TextIO) -> bool:
	workspace, path, check = args

	original = (workspace / path).read_text()
	formatted = bdlFormatter(formatType="bdl", source=str(workspace / path))

	if original != formatted:
		if check:
			stdout.write("This file is not formatted properly.\n")
			return False
		else:
			(workspace / path).write_text(formatted, encoding="utf-8")

	return True


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bdl formatter/linter.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(args.context, workload, endswith=[".bdl"])

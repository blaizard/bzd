import argparse
import pathlib
import typing
import json

from bzd_sanitizer.worker import worker


def workload(args: typing.Tuple[pathlib.Path, pathlib.Path, bool], stdout: typing.TextIO) -> bool:
	workspace, path, check = args

	original = (workspace / path).read_text()
	formatted = json.dumps(json.loads(original), indent=4, sort_keys=True)

	if original != formatted:
		if check:
			stdout.write("This file is not formatted properly.\n")
			return False
		else:
			(workspace / path).write_text(formatted)

	return True


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Json formatter/linter.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	worker(args.context, workload, endswith=[".json"])

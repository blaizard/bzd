import argparse
import pathlib
import sys

from tools.shell.shell import buildAll
from bzd_sanitizer.context import Context

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Tool for generating shell helper files.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	context = Context.fromFile(args.context)

	buildAll(workspace=context.workspace)

	sys.exit(0)

import argparse
import pathlib
import sys

from tools.shell.shell import buildAll

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Tool checking and formating json files.")
	parser.add_argument("workspace", type=pathlib.Path, help="Workspace to be processed.")

	args = parser.parse_args()

	buildAll(workspace=args.workspace)

	sys.exit(0)

import argparse
import os
import pathlib
import typing

IGNORED_DIRECTORIES = {".git"}


def findFiles(workspace: pathlib.Path, name: str) -> typing.Iterator[pathlib.Path]:
	"""Enumerate files matching the given name, relative to the workspace."""

	for root, directories, files in os.walk(workspace):
		directories[:] = [d for d in directories if d not in IGNORED_DIRECTORIES and not d.startswith("bazel-")]
		if name in files:
			yield pathlib.Path(root, name).relative_to(workspace)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bazel dependency updater.")
	parser.add_argument("workspace", type=pathlib.Path, help="The workspace path.")
	args = parser.parse_args()

	for path in findFiles(args.workspace, "MODULE.bazel"):
		print(path)

import os
import pathlib
import typing

IGNORED_DIRECTORIES = {".git"}


def findFiles(workspace: pathlib.Path, name: str) -> typing.Iterator[pathlib.Path]:
	"""Enumerate files matching the given name, relative to the workspace."""

	for root, directories, files in os.walk(workspace):
		directories[:] = sorted(
			directory for directory in directories if directory not in IGNORED_DIRECTORIES and not directory.startswith("bazel-")
		)
		if name in files:
			yield pathlib.Path(root, name).relative_to(workspace)

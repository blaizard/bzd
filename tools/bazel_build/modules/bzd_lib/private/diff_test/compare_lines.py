"""Comparator for text files using unified diff output."""

import difflib
import pathlib

from private.diff_test.common import getShortestDistinctPaths, maybeColorizeLine


def compare(file1: pathlib.Path, file2: pathlib.Path, color: bool) -> bool:

	content1 = file1.read_text()
	content2 = file2.read_text()

	if content1 == content2:
		return True

	shortest1, shortest2 = getShortestDistinctPaths(file1, file2)

	diff = difflib.unified_diff(
		content1.splitlines(keepends=True),
		content2.splitlines(keepends=True),
		fromfile=str(shortest1),
		tofile=str(shortest2),
	)

	colors = {
		"-": "\033[0;31m",
		"+": "\033[0;32m",
		"@": "\033[0;34m",
	}

	for line in diff:
		prefix = line[0] if line else ""
		color = colors.get(prefix, None)
		print(maybeColorizeLine(line.rstrip("\n"), color))

	return False

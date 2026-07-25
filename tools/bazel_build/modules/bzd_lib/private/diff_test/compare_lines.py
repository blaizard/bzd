"""Comparator for text files using unified diff output."""

import pathlib

from private.diff_test.common import printUnifiedDiff


def compare(file1: pathlib.Path, file2: pathlib.Path, color: bool) -> bool:

	content1 = file1.read_text()
	content2 = file2.read_text()

	if content1 == content2:
		return True

	printUnifiedDiff(content1.splitlines(keepends=True), content2.splitlines(keepends=True), file1, file2, color)

	return False

"""Common utilities."""

import pathlib
import typing
import difflib


def getShortestDistinctPaths(path1: pathlib.Path, path2: pathlib.Path) -> typing.Tuple[pathlib.Path, pathlib.Path]:
	"""Get shortest distinct path given 2 paths."""

	shortest1 = []
	shortest2 = []
	for p1, p2 in zip(path1.parts[::-1], path2.parts[::-1]):
		shortest1.append(p1)
		shortest2.append(p2)
		if p1 != p2:
			break
	return pathlib.Path(*shortest1[::-1]), pathlib.Path(*shortest2[::-1])


def maybeColorizeLine(text: str, color: typing.Optional[str]) -> str:
	"""Colorize the line if a color is set."""

	return f"{color}{text}\033[0m" if color else text


def printUnifiedDiff(
	content1: typing.List[str], content2: typing.List[str], file1: pathlib.Path, file2: pathlib.Path, color: bool
) -> None:
	"""Print a diff between 2 list of strings."""

	shortest1, shortest2 = getShortestDistinctPaths(file1, file2)
	diff = list(difflib.unified_diff(content1, content2, fromfile=shortest1.as_posix(), tofile=shortest2.as_posix()))

	colors = {
		"-": "\033[0;31m",
		"+": "\033[0;32m",
		"@": "\033[0;34m",
	}

	for line in diff:
		prefix = line[0] if line else ""
		maybeColorStr = colors.get(prefix, None) if color else None
		print(maybeColorizeLine(line.rstrip("\n"), maybeColorStr))

"""Common utilities."""

import pathlib
import typing


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

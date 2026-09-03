import pathlib

from python.runfiles import runfiles


def pathFromRLocation(rlocation: str) -> pathlib.Path:
	"""Returns the path to a file in the runfiles directory given its rlocation."""

	return pathlib.Path(runfiles.Create().Rlocation(rlocation))  # type: ignore

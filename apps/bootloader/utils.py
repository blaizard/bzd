import pathlib
import tempfile
import time
import typing


class RollingNamedTemporaryFile:

	def __init__(self, namespace: str, maxFiles: int) -> None:
		"""Constructor for the RollingNamedTemporaryFile object.

		The namespace can have up-to "maxFiles" files, older files are deleted.

		Args:
			namespace: The namespace to refer to.
			maxFiles: Keep maximum a certain number of files, older ones will be deleted.
		"""
		self.temporaryDirectory = pathlib.Path(tempfile.gettempdir()) / f"namespace_{namespace}"
		self.maxFiles = maxFiles

		self.temporaryDirectory.mkdir(exist_ok=True)

	def get(self) -> pathlib.Path:
		"""Get a named temporary file from a namespace.

		Returns:
			The full path of the temporary file.
		"""

		# Generate a unique file name.
		while True:
			path = self.temporaryDirectory / f"tmp.{time.time()}"
			if not path.exists():
				path.touch()
				break

		# Get all the files and sort them by modification date.
		latestFiles = self.all()

		# Keep only the "maxFiles" newest files.
		while len(latestFiles) > self.maxFiles:
			latestFiles.pop().unlink()

		return path

	def all(self) -> typing.List[pathlib.Path]:
		"""Get all the files sorted by modification date, the newest the first."""

		allFiles = self.temporaryDirectory.glob("tmp.*")
		return sorted(allFiles, reverse=True)

	def reset(self) -> None:
		"""Delete all temporary files associated with this namespace."""

		for f in self.all():
			f.unlink()

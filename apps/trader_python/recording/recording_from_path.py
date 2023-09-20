import typing
import pathlib

from apps.trader_python.recording.recording import Price, RecordingPair, Recording


def _readCSV(path: pathlib.Path, separator: str) -> typing.Iterator[typing.List[str]]:
	with path.open(mode="r") as f:
		while True:
			rawline = f.readline()
			if not rawline:
				break
			line = rawline.strip()
			if line:
				yield line.split(separator)


class RecordingPairFromPath(RecordingPair):

	def __init__(self, first: str, second: str, path: pathlib.Path) -> None:
		super().__init__(first, second)
		self.path = path
		self._buildIndex(self.path)

	def _buildIndex(self, path: pathlib.Path) -> None:
		"""Build the index from the given path."""

		assert path.is_dir(), f"The path '{path}' must point to a directory."
		self.files = sorted(list(d for d in path.iterdir() if d.is_file()))

	@staticmethod
	def readPriceFromFile(path: pathlib.Path) -> typing.Iterator[Price]:
		for price in _readCSV(path, separator=";"):
			if len(price) < 3:
				pass
			yield Price(timestamp=int(price[0]), price=float(price[1]), volume=float(price[2]))

	def __iter__(self) -> typing.Iterator[Price]:
		for f in self.files:
			for price in RecordingPairFromPath.readPriceFromFile(f):
				yield price


class RecordingFromPath(Recording):

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path.expanduser().resolve()
		self._buildIndex(self.path)

	def _buildIndex(self, path: pathlib.Path) -> None:
		"""Build the index from the given path."""

		assert path.is_dir(), f"The path '{path}' must point to a directory."

		# Look for the pairs.
		self.pairs = []
		directories = list(d for d in path.iterdir() if d.is_dir())
		for d in directories:
			splitName = d.name.split("-")
			assert len(splitName) == 2, "This directory '{d}' is not a valid pair recording."
			pair = RecordingPairFromPath(splitName[0], splitName[1], d)
			self.pairs.append(pair)

	def __iter__(self) -> typing.Iterator[RecordingPair]:
		for pair in self.pairs:
			yield pair

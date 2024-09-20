import typing
import pathlib
import json

from apps.trader_python.recording.recording import Info, Event, EventKind, OHLC, RecordingPair, Recording


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

		# Build the index from the given path.
		assert self.path.is_dir(), f"The path '{self.path}' must point to a directory."
		self.files = sorted(list(d for d in self.path.iterdir() if d.is_file() and d.name not in ["info.json"]))

		# Load the info
		if (self.path / "info.json").is_file():
			self.info = Info(**json.loads((self.path / "info.json").read_text()))

	@staticmethod
	def readOHLCFromFile(path: pathlib.Path) -> typing.Iterator[OHLC]:
		for data in _readCSV(path, separator=";"):
			if len(data) < 6:
				pass
			events = [
			    Event(EventKind(data[index]), data[index + 1])  # type: ignore
			    for index in range(6, len(data), 2)
			]
			yield OHLC(timestamp=int(data[0]),
			           open=float(data[1]),
			           high=float(data[2]),
			           low=float(data[3]),
			           close=float(data[4]),
			           volume=float(data[5]),
			           events=events)

	def __iter__(self) -> typing.Iterator[OHLC]:
		for f in self.files:
			for ohlc in RecordingPairFromPath.readOHLCFromFile(f):
				yield ohlc


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

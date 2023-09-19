import typing
import pathlib

from apps.trader_python.recording.recording import Price, RecordingPair


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

	@staticmethod
	def readPriceFromFile(path: pathlib.Path) -> typing.Iterator[Price]:
		for price in _readCSV(path, separator=";"):
			if len(price) < 3:
				pass
			yield Price(timestamp=int(price[0]), price=float(price[1]), volume=float(price[2]))

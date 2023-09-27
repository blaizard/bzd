import bisect
import typing

from apps.trader_python.recording.recording import Recording, RecordingPair, EventKind, Event, OHLC


class RecordingPairFromData(RecordingPair):

	def __init__(self, *args, **kwargs) -> None:
		super().__init__(*args, **kwargs)
		self.prices: typing.List[OHLC] = []

	@staticmethod
	def fromPrices(prices: typing.Iterable[OHLC], *args, **kwargs) -> "RecordingPairFromData":
		pair = RecordingPairFromData(*args, **kwargs)
		pair.prices = sorted(prices, key=lambda x: x.timestamp)
		return pair

	def addOHLC(self, timestamp: int, open: float, high: float, low: float, close: float, volume: float,
	            events: typing.Iterable[Event]) -> None:
		"""Add a OHLC entry."""

		self.addOHLCObject(
		    OHLC(timestamp=timestamp, open=open, high=high, low=low, close=close, volume=volume, events=[*events]))

	def addOHLCObject(self, ohlc: OHLC, ignoreEqual: bool = False) -> None:
		"""Add a OHLC object entry."""

		index = bisect.bisect_left(self.prices, ohlc.timestamp, key=lambda x: x.timestamp)
		if index != len(self.prices) and self.prices[index].timestamp == ohlc.timestamp:
			assert ignoreEqual, f"The OHLC {ohlc.date} already exists."
		else:
			self.prices.insert(index, ohlc)

	def __iter__(self) -> typing.Iterator[OHLC]:
		for ohlc in self.prices:
			yield ohlc


class RecordingFromData(Recording):

	def __init__(self) -> None:
		super().__init__()
		self.pairs: typing.Dict[str, RecordingPair] = {}

	def addPair(self, recordingPair: RecordingPair) -> None:
		assert recordingPair.uid not in self.pairs, f"The recording pair '{recordingPair.uid}' is already set."
		self.pairs[recordingPair.uid] = recordingPair

	def __iter__(self) -> typing.Iterator[RecordingPair]:
		for pair in self.pairs.values():
			yield pair

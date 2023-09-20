import bisect
import typing

from apps.trader_python.recording.recording import Recording, RecordingPair, EventKind, Event, Price


class RecordingPairFromData(RecordingPair):

	def __init__(self, first: str = "x", second: str = "y") -> None:
		super().__init__(first, second)
		self.prices: typing.List[Price] = []
		self.events: typing.List[Event] = []

	@staticmethod
	def fromPrices(prices: typing.Iterable[Price], *args, **kwargs) -> "RecordingPairFromData":
		pair = RecordingPairFromData(*args, **kwargs)
		pair.prices = sorted(prices, key=lambda x: x.timestamp)
		return pair

	def addPrice(self, timestamp: int, price: float, volume: float, ignoreEqual: bool = False) -> None:
		"""Add a price entry."""

		self.addPriceObject(Price(timestamp, price, volume), ignoreEqual=ignoreEqual)

	def addPriceObject(self, price: Price, ignoreEqual: bool = False) -> None:
		"""Add a price object entry."""

		index = bisect.bisect_left(self.prices, price.timestamp, key=lambda x: x.timestamp)
		if index != len(self.prices) and self.prices[index].timestamp == price.timestamp:
			assert ignoreEqual, f"The entry {price.date} already exists."
		else:
			self.prices.insert(index, price)

	def addEvent(self, timestamp: int, kind: EventKind, value: float) -> None:
		"""Add an event related too this stock."""

		bisect.insort(self.events, Event(timestamp, kind, value), key=lambda x: x.timestamp)

	def __iter__(self) -> typing.Iterator[Price]:
		for price in self.prices:
			yield price


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

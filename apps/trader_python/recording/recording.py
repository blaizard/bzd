import pathlib
import enum
import datetime
import dataclasses
import typing


class EventKind(enum.Enum):
	SPLIT = "split"
	DIVIDEND = "dividend"


@dataclasses.dataclass
class Event:
	kind: EventKind
	value: float


@dataclasses.dataclass
class OHLC:
	timestamp: int
	open: float
	high: float
	low: float
	close: float
	volume: float
	events: typing.List[Event]

	@property
	def price(self) -> float:
		return (self.open + self.close) / 2.

	@property
	def date(self) -> datetime.datetime:
		return datetime.datetime.utcfromtimestamp(self.timestamp)


@dataclasses.dataclass
class Info:
	name: typing.Optional[str] = None
	timeZone: typing.Optional[str] = None
	countryHQ: typing.Optional[str] = None
	industries: typing.List[str] = dataclasses.field(default_factory=lambda: [])
	sectors: typing.List[str] = dataclasses.field(default_factory=lambda: [])
	employees: typing.Optional[int] = None
	resolution: typing.Optional[int] = None

	def toDict(self) -> typing.Dict[str, typing.Any]:
		return dataclasses.asdict(self)


class RecordingPair:

	def __init__(self, first: str, second: str, info: Info = Info()) -> None:
		self.first = first.upper().replace("-", "_")
		self.second = second.upper().replace("-", "_")
		self.info = info

	@property
	def uid(self) -> str:
		return f"{self.first}/{self.second}"

	def __iter__(self) -> typing.Iterator[OHLC]:
		raise Exception("Implement __iter__.")

	def __repr__(self) -> str:
		"""String representation of a recording pair."""

		def ohlcToStr(ohlc: OHLC) -> str:
			content = f"{str(ohlc.date)}\t{ohlc.open:>10}\t{ohlc.high:>10}\t{ohlc.low:>10}\t{ohlc.close:>10}\t{ohlc.volume:>20}"
			for event in ohlc.events:
				content += f"\t{event.kind:>15}\t{event.value:>20}"
			return content

		def ohlcsToStr(prices: typing.List[OHLC]) -> str:
			return "\n".join([ohlcToStr(price) for price in prices])

		content = self.uid + ":\n"
		content += "\n".join([f"\t{k} = {str(v)}" for k, v in self.info.toDict().items() if v]) + "\n"
		data = [*self]
		if len(data) > 10:
			content += ohlcsToStr(data[0:5]) + "\n...\n" + ohlcsToStr(data[-5:])
		else:
			content += ohlcsToStr(data)
		return content


class Recording:

	def __iter__(self) -> typing.Iterator[RecordingPair]:
		raise Exception("Implement __iter__.")

	def __repr__(self) -> str:
		"""String representation of a recording."""

		content = []
		for pair in self:
			content.append(str(pair))
		return "\n\n".join(content)

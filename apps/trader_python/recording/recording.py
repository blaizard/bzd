import dataclasses
import datetime
import enum
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
	events: list[Event]

	@property
	def price(self) -> float:
		return (self.open + self.close) / 2.0

	@property
	def date(self) -> datetime.datetime:
		return datetime.datetime.fromtimestamp(self.timestamp, tz=datetime.UTC)


@dataclasses.dataclass
class Info:
	name: str | None = None
	timeZone: str | None = None
	countryHQ: str | None = None
	industries: list[str] = dataclasses.field(default_factory=list)
	sectors: list[str] = dataclasses.field(default_factory=list)
	employees: int | None = None
	resolution: int | None = None

	def toDict(self) -> dict[str, typing.Any]:
		return dataclasses.asdict(self)


_defaultInfo = Info()


class RecordingPair:
	def __init__(self, first: str, second: str, info: Info = _defaultInfo) -> None:
		self.first = first.upper().replace("-", "_")
		self.second = second.upper().replace("-", "_")
		self.info = info

	@property
	def uid(self) -> str:
		return f"{self.first}/{self.second}"

	def __iter__(self) -> typing.Iterator[OHLC]:
		raise RuntimeError("Implement __iter__.")

	def __repr__(self) -> str:
		"""String representation of a recording pair."""

		def ohlcToStr(ohlc: OHLC) -> str:
			content = f"{ohlc.date!s}\t{ohlc.open:>10}\t{ohlc.high:>10}\t{ohlc.low:>10}\t{ohlc.close:>10}\t{ohlc.volume:>20}"
			for event in ohlc.events:
				content += f"\t{event.kind:>15}\t{event.value:>20}"
			return content

		def ohlcsToStr(prices: list[OHLC]) -> str:
			return "\n".join([ohlcToStr(price) for price in prices])

		content = self.uid + ":\n"
		content += "\n".join([f"\t{k} = {v!s}" for k, v in self.info.toDict().items() if v]) + "\n"
		data = [*self]
		if len(data) > 10:
			content += ohlcsToStr(data[0:5]) + "\n...\n" + ohlcsToStr(data[-5:])
		else:
			content += ohlcsToStr(data)
		return content


class Recording:
	def __iter__(self) -> typing.Iterator[RecordingPair]:
		raise RuntimeError("Implement __iter__.")

	def __repr__(self) -> str:
		"""String representation of a recording."""

		content = []
		for pair in self:
			content.append(str(pair))
		return "\n\n".join(content)

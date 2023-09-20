import pathlib
import enum
import datetime
import dataclasses
import typing


class EventKind(enum.Enum):
	SPLIT: 1
	DIVIDEND: 2


@dataclasses.dataclass
class Event:
	timestamp: int
	kind: EventKind
	value: float

	@property
	def date(self) -> datetime.datetime:
		return datetime.datetime.utcfromtimestamp(self.timestamp)


@dataclasses.dataclass
class Price:
	timestamp: int
	price: float
	volume: float

	@property
	def date(self) -> datetime.datetime:
		return datetime.datetime.utcfromtimestamp(self.timestamp)


class RecordingPair:

	def __init__(self, first: str, second: str) -> None:
		self.first = first.upper().replace("-", "_")
		self.second = second.upper().replace("-", "_")

	@property
	def uid(self) -> str:
		return f"{self.first}/{self.second}"

	def __repr__(self) -> str:
		"""String representation of a recording pair."""

		content = self.uid + ":\n"
		data = []
		for price in self:
			data.append(f"{str(price.date)}\t{price.price:>20}\t{price.volume:>20}")
		if len(data) > 10:
			content += "\n".join(data[0:5]) + "\n...\n" + "\n".join(data[-5:])
		else:
			content += "\n".join(data)
		return content


class Recording:

	def __repr__(self) -> str:
		"""String representation of a recording."""

		content = []
		for pair in self:
			content.append(str(pair))
		return "\n\n".join(content)

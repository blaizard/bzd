"""Virtual exchange."""

import typing

from apps.trader_python.exchange.pair import Pair
from apps.trader_python.exchange.virtual.pair import VirtualPair
from apps.trader_python.recording.recording import Recording


class Virtual:

	def __init__(self, recording: Recording) -> None:
		self.recording = recording

	def getPairs(self) -> typing.Iterable[Pair]:

		pair = Pair("USD", "EUR", VirtualPair())

		return []

"""Virtual exchange."""

import typing

from apps.trader_python.exchange.pair import Pair
from apps.trader_python.exchange.virtual.pair import VirtualPair
from apps.trader_python.recording.recording import Recording


class Virtual:

	def __init__(self, recording: Recording) -> None:
		self.recording = recording
		self.pairs = [VirtualPair(pair) for pair in self.recording]

	def getPairs(self) -> typing.Iterator[Pair]:

		for pair in self.pairs:
			yield typing.cast(Pair, pair)

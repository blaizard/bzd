"""Exchange interface."""

import typing

from apps.trader_python.exchange.pair import Pair


class Exchange(typing.Protocol):

	def start(self) -> None:
		pass

	def stop(self) -> None:
		pass

	def getPairs(self) -> typing.Iterable[Pair]:
		"""Get the list of all available pairs for this exchange."""

"""Exchange interface."""

import typing


class Exchange(typing.Protocol):

	def start(self) -> None:
		pass

	def stop(self) -> None:
		pass

	def getPairs(self) -> typing.List[str]:
		pass

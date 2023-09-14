import typing


class Pair(typing.Protocol):

	@property
	def first(self) -> str:
		"""."""

	@property
	def second(self) -> str:
		"""."""

	def positionLong(self) -> None:
		"""Set a long position on this pair."""

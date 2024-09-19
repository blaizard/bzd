import typing


@typing.runtime_checkable
class PairImpl(typing.Protocol):
	pass


@typing.runtime_checkable
class PairLongImpl(PairImpl, typing.Protocol):

	def makeLong(self) -> None:
		"""Set a long position on this pair."""


@typing.runtime_checkable
class PairShortImpl(PairImpl, typing.Protocol):

	def makeShort(self) -> None:
		"""Set a short position on this pair."""


class Pair:

	def __init__(self, first: str, second: str, impl: typing.Union[PairLongImpl, PairShortImpl]) -> None:
		self.first = first
		self.second = second
		self.impl = impl
		self.isShort = isinstance(impl, PairShortImpl)
		self.isLong = isinstance(impl, PairLongImpl)

	def makeLong(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		assert isinstance(self.impl, PairLongImpl)
		self.impl.makeLong(*args, **kwargs)

	def makeShort(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		assert isinstance(self.impl, PairShortImpl)
		self.impl.makeShort(*args, **kwargs)

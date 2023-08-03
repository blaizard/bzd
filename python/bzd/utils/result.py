import typing

V = typing.TypeVar("V")
E = typing.TypeVar("E")


class Result(typing.Generic[V, E]):
	"""
    Result class.
    """

	def __init__(self, value: V) -> None:
		self.value_: typing.Optional[V] = value
		self.error_: typing.Optional[E] = None

	U = typing.TypeVar("U", bound="Result[V, E]")

	@classmethod
	def makeError(cls: typing.Type[U], error: E) -> U:
		result = cls(value=typing.cast(V, None))
		result.error_ = error
		return result

	def __bool__(self) -> bool:
		return self.error_ is None

	@property
	def value(self) -> V:
		assert (self.value_ is not None), f"Cannot access the value of a result containing an error: {self.error}"
		return self.value_

	@property
	def error(self) -> E:
		assert (self.error_ is not None), "Cannot access the error of a result containing a value."
		return self.error_

	def __repr__(self) -> str:
		if bool(self):
			return f'<Result value="{str(self.value)}">'
		return f'<Result error="{str(self.error)}">'

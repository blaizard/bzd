import typing

V = typing.TypeVar("V")
E = typing.TypeVar("E")


class Result(typing.Generic[V, E]):
	"""Container for either a value or an error."""

	def __init__(self, value: V) -> None:
		self.value_: V | None = value
		self.error_: E | None = None

	U = typing.TypeVar("U", bound="Result[V, E]")

	@classmethod
	def makeError(cls, error: E) -> typing.Self:
		result = cls(value=typing.cast(V, None))
		result.error_ = error
		return result

	def __bool__(self) -> bool:
		return self.error_ is None

	def hasValue(self) -> bool:
		return bool(self)

	def hasError(self) -> bool:
		return not bool(self)

	@property
	def value(self) -> V:
		assert self.value_ is not None, f"Cannot access the value of a result containing an error: {self.error}"
		return self.value_

	@property
	def error(self) -> E:
		assert self.error_ is not None, "Cannot access the error of a result containing a value."
		return self.error_

	def __repr__(self) -> str:
		if bool(self):
			return f'<Result value="{self.value!s}">'
		return f'<Result error="{self.error!s}">'

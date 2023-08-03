import typing

from bzd.validation.schema import ProcessedResult, Values, ValuesList, ValuesDict

T = typing.TypeVar("T")


class Result(typing.Generic[T]):

	def __init__(self, isList: bool) -> None:
		self.isList = isList
		self.globalErrors: typing.List[str] = []
		self.errors: typing.Dict[str, typing.List[str]] = {}
		self.values_: Values = [] if self.isList else {}

	def addGlobalError(self, error: str) -> None:
		self.globalErrors.append(error)

	def addError(self, key: str, errors: typing.List[str]) -> None:
		assert key not in self.errors, "Key '{}' already assigned.".format(key)
		self.errors[key] = errors

	def addResult(self, key: str, result: ProcessedResult) -> None:
		if result:
			if self.isList:
				self.values_.append(result.value)  # type: ignore
			else:
				assert key not in self.values_, "Value already set."
				self.values_[key] = result.value  # type: ignore
		else:
			self.addError(key, result.errors)

	@property
	def values(self) -> T:
		assert bool(self), "Invalid result."
		return self.values_  # type: ignore

	def __bool__(self) -> bool:
		return not bool(self.errors) and not bool(self.globalErrors)

	def __repr__(self) -> str:
		if bool(self):
			return str(self.values_)

		if self.isList:
			FORMAT_NOT_VALIDATE_SINGLE = "position {key} does not validate: {message}"
			FORMAT_NOT_VALIDATE_MULTI = ("Some positional values do not validate: {message}")
		else:
			FORMAT_NOT_VALIDATE_SINGLE = "'{key}' does not validate: {message}"
			FORMAT_NOT_VALIDATE_MULTI = "Some values do not validate: {message}"

		content = []
		if len(self.globalErrors):
			content.extend(self.globalErrors)

		if len(self.errors.keys()) == 1:
			for key, result in self.errors.items():
				content.append(FORMAT_NOT_VALIDATE_SINGLE.format(key=key, message=", ".join(result)))
		else:
			messages = ['{}: "{}"'.format(key, ", ".join(result)) for key, result in self.errors.items()]
			content.append(FORMAT_NOT_VALIDATE_MULTI.format(message="; ".join(messages)))

		return "; ".join(content) if content else "no errors"

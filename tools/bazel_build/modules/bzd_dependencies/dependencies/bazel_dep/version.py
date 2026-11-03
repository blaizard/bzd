import functools
import re
import typing

_VERSION_PATTERN = re.compile(r"^(\d+(?:\.\d+)*)(?:-([0-9A-Za-z.-]+))?(?:\+[0-9A-Za-z.-]+)?$")


@functools.total_ordering
class Version:
	"""Version of a module, as used by the Bazel Central Registry."""

	def __init__(
		self,
		value: str,
		numeric: typing.Tuple[int, ...],
		prerelease: typing.Tuple[typing.Union[int, str], ...],
	) -> None:
		self.value = value
		self.numeric_ = numeric
		self.prerelease_ = prerelease

	@classmethod
	def parse(cls, value: str) -> typing.Optional["Version"]:
		"""Parse a version, returning None if it does not follow the registry format."""

		match = _VERSION_PATTERN.match(value)
		if match is None:
			return None
		numeric = tuple(int(component) for component in match.group(1).split("."))
		prereleaseString = match.group(2)
		prerelease: typing.Tuple[typing.Union[int, str], ...] = ()
		if prereleaseString is not None:
			prerelease = tuple(
				int(identifier) if identifier.isdigit() else identifier for identifier in prereleaseString.split(".")
			)
		return cls(value, numeric, prerelease)

	@property
	def isStable(self) -> bool:
		"""Whether the version is released, as opposed to a pre-release."""

		return not self.prerelease_

	def __str__(self) -> str:
		return self.value

	def __eq__(self, other: object) -> bool:
		if not isinstance(other, Version):
			return NotImplemented
		return self._comparisonKey() == other._comparisonKey()

	def __lt__(self, other: "Version") -> bool:
		return self._comparisonKey() < other._comparisonKey()

	def __hash__(self) -> int:
		return hash(self._comparisonKey())

	def _comparisonKey(self) -> typing.Tuple[typing.Any, ...]:
		return (
			self.numeric_,
			# A release is newer than any of its pre-releases.
			0 if self.prerelease_ else 1,
			tuple((0, identifier, "") if isinstance(identifier, int) else (1, 0, identifier) for identifier in self.prerelease_),
		)

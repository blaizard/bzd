import re
import typing
from dataclasses import dataclass

_REGEXPR_SEMVER = re.compile(
    r"(?P<major>[0-9]+)(?:\.(?P<minor>[0-9]+)(?:\.(?P<patch>[0-9]+))?)?(?:\-(?P<preRelease>[^\s+]+))?(?:\+(?P<build>\S+))?"
)
_REGEXPR_CONSTRAINT = re.compile(
    r"(?P<operator>\^|~|=|!=|>|>=|<|<=)?\s*(?P<major>[0-9xX*]+)(?:\.(?P<minor>[0-9xX*]+)(?:\.(?P<patch>[0-9xX*]+))?)?(?:\-(?P<preRelease>[^\s+]+))?(?:\+(?P<build>\S+))?"
)
_MAX_VERSION_INT = 999999999999


@dataclass
class Semver:
	"""Describe a fixed version according to Semantic Versioning 2.0.0
	
	See: https://semver.org/
	"""

	major: int = 0
	minor: int = 0
	patch: int = 0
	preRelease: str = ""
	build: str = ""

	@staticmethod
	def fromString(string: str) -> "Semver":
		m = _REGEXPR_SEMVER.match(string.strip())
		assert m is not None, f"The version string is malformed: '{string}'."
		return Semver(major=int(m.group("major") or 0),
		              minor=int(m.group("minor") or 0),
		              patch=int(m.group("patch") or 0),
		              preRelease=m.group("preRelease") or "",
		              build=m.group("build") or "")

	@property
	def core(self) -> str:
		"""Get the semver core version as a string."""

		return f"{self.major}.{self.minor}.{self.patch}"

	def __eq__(self, other: object) -> bool:
		if not isinstance(other, Semver):
			return NotImplemented
		# We should not check the build: https://semver.org/spec/v2.0.0-rc.2.html#spec-item-10
		return self.major == other.major and self.minor == other.minor and self.patch == other.patch and self.preRelease == other.preRelease

	def __ne__(self, other: object) -> bool:
		if not isinstance(other, Semver):
			return NotImplemented
		return not (self == other)

	def __lt__(self, other: object) -> bool:
		if not isinstance(other, Semver):
			return NotImplemented
		# Implemented as described: https://semver.org/spec/v2.0.0-rc.2.html#spec-item-11
		if self.major != other.major:
			return self.major < other.major
		if self.minor != other.minor:
			return self.minor < other.minor
		if self.patch != other.patch:
			return self.patch < other.patch
		if self.preRelease != other.preRelease:
			# Handle 1.0-beta < 1.0
			if not self.preRelease or not other.preRelease:
				return self.preRelease > other.preRelease
			# Each pre release need to be split by '.'
			splitSelf = self.preRelease.split(".")
			splitOther = other.preRelease.split(".")
			for selfPart, otherPart in zip(splitSelf, splitOther):
				if selfPart != otherPart:
					if selfPart.isnumeric() and otherPart.isnumeric():
						return int(selfPart) < int(otherPart)
					return selfPart < otherPart
			if len(splitSelf) != len(splitOther):
				return len(splitSelf) < len(splitOther)
		return False

	def __le__(self, other: object) -> bool:
		if not isinstance(other, Semver):
			return NotImplemented
		return (self == other) or (self < other)

	def __gt__(self, other: object) -> bool:
		if not isinstance(other, Semver):
			return NotImplemented
		return not (self <= other)

	def __ge__(self, other: object) -> bool:
		if not isinstance(other, Semver):
			return NotImplemented
		return not (self < other)

	def __str__(self) -> str:
		version = self.core
		if self.preRelease:
			version += "-" + self.preRelease
		if self.build:
			version += "-" + self.build
		return version

	def __hash__(self) -> int:
		# Do not include self.build, see: https://semver.org/spec/v2.0.0-rc.2.html#spec-item-10
		return hash(self.core + self.preRelease)

	def __repr__(self) -> str:
		return f"<Semver {str(self)}>"


class SemverConstraint:

	def __init__(self, string: str) -> None:
		m = _REGEXPR_CONSTRAINT.match(string.strip())
		assert m is not None, f"The version constraint is malformed: '{string}'."
		self.operator = m.group("operator") or ""
		self.major = m.group("major") or "x"
		self.minor = m.group("minor") or "x"
		self.patch = m.group("patch") or "x"
		self.preRelease = m.group("preRelease") or ""
		self.build = m.group("build") or ""

	@staticmethod
	def isWildcard(value: str) -> bool:
		return value in (
		    "x",
		    "X",
		    "*",
		)

	@staticmethod
	def setIfWildcard(value: str, wildcard: int) -> int:
		if SemverConstraint.isWildcard(value):
			return wildcard
		return int(value)

	def _matchTilde(self, version: Semver) -> bool:
		minVersion = Semver(major=self.setIfWildcard(self.major, 0),
		                    minor=self.setIfWildcard(self.minor, 0),
		                    patch=self.setIfWildcard(self.patch, 0),
		                    preRelease=self.preRelease,
		                    build=self.build)
		maxVersion = Semver(major=self.setIfWildcard(self.major, _MAX_VERSION_INT),
		                    minor=self.setIfWildcard(self.minor, _MAX_VERSION_INT),
		                    patch=_MAX_VERSION_INT,
		                    preRelease=self.preRelease,
		                    build=self.build)
		return version >= minVersion and version <= maxVersion

	def _matchCaret(self, version: Semver) -> bool:
		minVersion = Semver(major=self.setIfWildcard(self.major, 0),
		                    minor=self.setIfWildcard(self.minor, 0),
		                    patch=self.setIfWildcard(self.patch, 0),
		                    preRelease=self.preRelease,
		                    build=self.build)

		maxVersions: typing.List[int] = []
		for v in [self.major, self.minor, self.patch]:
			if self.isWildcard(v):
				maxVersions[len(maxVersions) - 1] += 1
				break
			if int(v) > 0:
				maxVersions.append(int(v) + 1)
				break
			maxVersions.append(int(v))
		maxVersions += [0, 0, 0]

		maxVersion = Semver(major=maxVersions[0],
		                    minor=maxVersions[1],
		                    patch=maxVersions[2],
		                    preRelease=self.preRelease,
		                    build=self.build)
		return version >= minVersion and version < maxVersion

	def _matchEqual(self, version: Semver) -> bool:
		return version == Semver(major=self.setIfWildcard(self.major, 0),
		                         minor=self.setIfWildcard(self.minor, 0),
		                         patch=self.setIfWildcard(self.patch, 0),
		                         preRelease=self.preRelease,
		                         build=self.build)

	def _matchNotEqual(self, version: Semver) -> bool:
		return version != Semver(major=self.setIfWildcard(self.major, 0),
		                         minor=self.setIfWildcard(self.minor, 0),
		                         patch=self.setIfWildcard(self.patch, 0),
		                         preRelease=self.preRelease,
		                         build=self.build)

	def _matchGreater(self, version: Semver) -> bool:
		return version > Semver(major=self.setIfWildcard(self.major, 0),
		                        minor=self.setIfWildcard(self.minor, 0),
		                        patch=self.setIfWildcard(self.patch, 0),
		                        preRelease=self.preRelease,
		                        build=self.build)

	def _matchGreaterEqual(self, version: Semver) -> bool:
		return version >= Semver(major=self.setIfWildcard(self.major, 0),
		                         minor=self.setIfWildcard(self.minor, 0),
		                         patch=self.setIfWildcard(self.patch, 0),
		                         preRelease=self.preRelease,
		                         build=self.build)

	def _matchLess(self, version: Semver) -> bool:
		return version < Semver(major=self.setIfWildcard(self.major, _MAX_VERSION_INT),
		                        minor=self.setIfWildcard(self.minor, _MAX_VERSION_INT),
		                        patch=self.setIfWildcard(self.patch, _MAX_VERSION_INT),
		                        preRelease=self.preRelease,
		                        build=self.build)

	def _matchLessEqual(self, version: Semver) -> bool:
		return version <= Semver(major=self.setIfWildcard(self.major, _MAX_VERSION_INT),
		                         minor=self.setIfWildcard(self.minor, _MAX_VERSION_INT),
		                         patch=self.setIfWildcard(self.patch, _MAX_VERSION_INT),
		                         preRelease=self.preRelease,
		                         build=self.build)

	def _matchVersion(self, version: Semver) -> bool:
		if self.isWildcard(self.major):
			return version >= Semver(major=0, minor=0, patch=0, preRelease=self.preRelease, build=self.build)
		if self.isWildcard(self.minor):
			return version >= Semver(
			    major=int(
			        self.major), minor=0, patch=0, preRelease=self.preRelease, build=self.build) and version < Semver(
			            major=int(self.major) + 1, minor=0, patch=0, preRelease=self.preRelease, build=self.build)
		if self.isWildcard(self.patch):
			return version >= Semver(
			    major=int(self.major), minor=int(self.minor), patch=0, preRelease=self.preRelease,
			    build=self.build) and version < Semver(major=int(self.major),
			                                           minor=int(self.minor) + 1,
			                                           patch=0,
			                                           preRelease=self.preRelease,
			                                           build=self.build)

		return self._matchEqual(version=version)

	def match(self, version: Semver) -> bool:
		matcherMap = {
		    "~": self._matchTilde,
		    "^": self._matchCaret,
		    "=": self._matchEqual,
		    "!=": self._matchNotEqual,
		    ">": self._matchGreater,
		    ">=": self._matchGreaterEqual,
		    "<": self._matchLess,
		    "<=": self._matchLessEqual,
		}
		if self.operator:
			assert self.operator in matcherMap, f"The operator '{self.operator}' is not supported."
			return matcherMap[self.operator](version=version)
		return self._matchVersion(version=version)

	def __repr__(self) -> str:
		version = f"{self.operator}{self.major}.{self.minor}.{self.patch}"
		if self.preRelease:
			version += "-" + self.preRelease
		if self.build:
			version += "-" + self.build
		return version


class SemverMatcher:
	"""Match a string with a semver."""

	def __init__(self, string: str) -> None:
		self.constraints = [SemverConstraint(string)]

	def match(self, version: Semver) -> bool:
		"""Match the given version."""

		for constraint in self.constraints:
			return constraint.match(version)

		return False

	def matchLatest(self, versions: typing.Iterable[Semver]) -> typing.Optional[Semver]:
		"""Match the latest version from the list."""

		sortedVersions = sorted(versions, reverse=True)
		for version in sortedVersions:
			if self.match(version=version):
				return version
		return None

	def __repr__(self) -> str:
		return f"<SemverMatcher {self.constraints}>"

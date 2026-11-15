import typing

from context.context import Context


class Dependencies:
	"""Set of dependencies, each identified by a name and a version."""

	def __init__(self) -> None:
		self.dependencies_: dict[tuple[str, str], list[typing.Any]] = {}

	def add(self, name: str, version: str, extra: typing.Any) -> None:
		self.dependencies_.setdefault((name, version), []).append(extra)

	def items(self) -> typing.Iterator[tuple[str, str, list[typing.Any]]]:
		for (name, version), extras in self.dependencies_.items():
			yield name, version, extras

	def __bool__(self) -> bool:
		return bool(self.dependencies_)

	def diff(self, other: "Dependencies") -> list[str]:
		"""Get the human-readable differences with another set of dependencies."""

		before = self._versionsByName()
		after = other._versionsByName()
		return [
			f"{name}: {', '.join(sorted(before[name]))} -> {', '.join(sorted(after[name]))}"
			for name in sorted(before.keys() & after.keys())
			if sorted(before[name]) != sorted(after[name])
		]

	def __str__(self) -> str:
		return "\n".join(f"{name}: {version}" for name, version in sorted(self.dependencies_.keys()))

	def _versionsByName(self) -> dict[str, list[str]]:
		versions: dict[str, list[str]] = {}
		for name, version, _ in self.items():
			versions.setdefault(name, []).append(version)
		return versions


class DependencyInterface(typing.Protocol):
	def __init__(self, context: Context) -> None: ...

	def get(self) -> Dependencies: ...

	def upgrade(self, dependencies: Dependencies) -> None: ...

import pathlib
import typing


class Dependencies:
	"""Set of dependencies, each identified by a name and a version."""

	def __init__(self) -> None:
		self.dependencies_: typing.Set[typing.Tuple[str, str]] = set()

	def add(self, name: str, version: str) -> None:
		self.dependencies_.add((name, version))

	def __str__(self) -> str:
		return "\n".join(f"{name}: {version}" for name, version in sorted(self.dependencies_))


class DependencyInterface(typing.Protocol):
	def __init__(self, workspace: pathlib.Path) -> None: ...

	def get(self) -> Dependencies: ...

	def upgrade(self) -> None: ...

import ast
import collections
import pathlib
import typing

from bzd_dependencies.utils.files import findFiles

from dependencies.bazel_dep.bcr import BcrRegistry
from dependencies.interface import Dependencies


class _Location(typing.NamedTuple):
	"""Location of a version literal within a MODULE.bazel file."""

	path: pathlib.Path
	start: typing.Optional[int]
	end: typing.Optional[int]


def _lineStarts(content: bytes) -> typing.List[int]:
	"""Get the byte offset of the start of every line."""

	starts = [0]
	for line in content.split(b"\n")[:-1]:
		starts.append(starts[-1] + len(line) + 1)
	return starts


def _versionRange(
	content: bytes,
	lineStarts: typing.List[int],
	node: ast.Constant,
) -> typing.Tuple[typing.Optional[int], typing.Optional[int]]:
	"""Get the byte range of a version literal content, None if it cannot be edited safely."""

	if node.end_lineno is None or node.end_col_offset is None:
		return None, None
	start = lineStarts[node.lineno - 1] + node.col_offset
	end = lineStarts[node.end_lineno - 1] + node.end_col_offset
	literal = content[start:end]
	quote = literal[:1]
	if quote not in (b'"', b"'") or literal[-1:] != quote or literal[1:-1] != str(node.value).encode("utf-8"):
		return None, None
	return start + 1, end - 1


class BazelDep:
	"""Discover and upgrade bazel_dep entries declared in the workspace MODULE.bazel files."""

	def __init__(self, workspace: pathlib.Path, registry: typing.Optional[BcrRegistry] = None) -> None:
		self.workspace = workspace
		self.registry_ = registry or BcrRegistry()

	def get(self) -> Dependencies:
		"""Get all bazel_dep entries, one per occurrence, in discovery order."""

		dependencies = Dependencies()
		for path in findFiles(self.workspace, "MODULE.bazel"):
			modulePath = self.workspace / path
			content = modulePath.read_bytes()
			lineStarts = _lineStarts(content)
			for node in ast.walk(ast.parse(content.decode("utf-8"))):
				if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id == "bazel_dep":
					keywords = {keyword.arg: keyword.value for keyword in node.keywords}
					name = keywords.get("name")
					if not (isinstance(name, ast.Constant) and isinstance(name.value, str)):
						continue
					version = keywords.get("version")
					if isinstance(version, ast.Constant) and isinstance(version.value, str):
						start, end = _versionRange(content, lineStarts, version)
						dependencies.add(name.value, version.value, _Location(modulePath, start, end))
					else:
						dependencies.add(name.value, "", _Location(modulePath, None, None))
		return dependencies

	def upgrade(self, dependencies: Dependencies) -> None:
		"""Upgrade every bazel_dep entry to the latest stable version published in the BCR."""

		edits: typing.Dict[pathlib.Path, typing.List[typing.Tuple[int, int, str]]] = collections.defaultdict(list)

		for name, declaredVersion, occurrences in dependencies.items():
			latestVersion = self.registry_.getLatestStable(name, declaredVersion)
			if latestVersion is None:
				continue
			for occurrence in occurrences:
				if occurrence.start is None or occurrence.end is None:
					continue
				edits[occurrence.path].append((occurrence.start, occurrence.end, latestVersion))

		for path, pathEdits in edits.items():
			content = bytearray(path.read_bytes())
			for start, end, latestVersion in sorted(pathEdits, reverse=True):
				content[start:end] = latestVersion.encode("utf-8")
			path.write_bytes(bytes(content))

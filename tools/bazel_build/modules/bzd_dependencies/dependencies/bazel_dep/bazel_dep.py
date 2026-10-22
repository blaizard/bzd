import ast
import pathlib

from bzd_dependencies.utils.files import findFiles
from dependencies.interface import Dependencies


class BazelDep:
	"""Discover bazel_dep entries declared in the workspace MODULE.bazel files."""

	def __init__(self, workspace: pathlib.Path) -> None:
		self.workspace = workspace

	def get(self) -> Dependencies:
		"""Get all bazel_dep entries, one per occurrence, in discovery order."""

		dependencies = Dependencies()
		for path in findFiles(self.workspace, "MODULE.bazel"):
			tree = ast.parse((self.workspace / path).read_text())
			for node in ast.walk(tree):
				if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id == "bazel_dep":
					keywords = {keyword.arg: keyword.value for keyword in node.keywords}
					name = keywords.get("name")
					if isinstance(name, ast.Constant) and isinstance(name.value, str):
						version = keywords.get("version")
						versionValue = version.value if isinstance(version, ast.Constant) and isinstance(version.value, str) else ""
						dependencies.add(name.value, versionValue)
		return dependencies

	def upgrade(self) -> None:
		pass

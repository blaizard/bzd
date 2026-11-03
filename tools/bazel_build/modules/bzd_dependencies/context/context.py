import json
import os
import pathlib
import typing

from bzd.utils.run import ExecuteResult
from bzd.utils.run import localBazel as _localBazel

_IGNORED_DIRECTORIES = {".git", ".opencode", "node_modules", "vendor_src"}


class Context:
	"""Shared context for the dependency backends."""

	def __init__(self, workspace: pathlib.Path) -> None:
		self.workspace = workspace.resolve()

	def localBazel(self, *args: typing.Any, **kwargs: typing.Any) -> ExecuteResult:
		"""Run bazel from the workspace with a sanitized environment."""

		environment = os.environ.copy()
		# Prevent the CI wrapper from re-applying its run_under to nested invocations.
		environment.pop("BAZEL_RUN_UNDER", None)
		environment.pop("BAZELISK_SKIP_WRAPPER", None)
		kwargs["cwd"] = self.workspace
		kwargs["env"] = environment
		return _localBazel(*args, **kwargs)

	def findFiles(self, name: str) -> typing.Iterator[pathlib.Path]:
		"""Enumerate files matching the given name, relative to the workspace."""

		for root, directories, files in os.walk(self.workspace):
			directories[:] = sorted(
				directory
				for directory in directories
				if directory not in _IGNORED_DIRECTORIES and not directory.startswith("bazel-")
			)
			if name in files:
				yield pathlib.Path(root, name).relative_to(self.workspace)

	def getLocalBzlmod(self) -> typing.Dict[str, pathlib.Path]:
		"""Get the local bzlmod repositories mapped under the workspace."""

		result = self.localBazel(
			["mod", "show_repo", "--all_visible_repos", "--output=streamed_jsonproto"],
			timeoutS=None,
		)
		repos: typing.Dict[str, pathlib.Path] = {}
		for line in result.getStdout().splitlines():
			if not line.strip():
				continue
			entry = json.loads(line)
			if entry.get("repoRuleName") != "local_repository":
				continue
			path = next(
				(attribute.get("stringValue") for attribute in entry.get("attribute", []) if attribute.get("name") == "path"),
				None,
			)
			root = (self.workspace / path).resolve() if path is not None else None
			if root is not None and root.is_relative_to(self.workspace):
				repos[entry["apparentName"].removeprefix("@")] = root
		return repos

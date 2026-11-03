import pathlib
import re
import shlex
import typing

from context.context import Context

from dependencies.interface import Dependencies

_PIN = re.compile(r"^([A-Za-z0-9][A-Za-z0-9._-]*)(?:\[[^\]]*\])?\s*==\s*(\S+)")
_LOCATION = re.compile(r"^(.*?):\d+:\d+: .*? (\S+)$")


class PipRequirements:
	"""Discover and upgrade the locks managed by compile_pip_requirements rules."""

	def __init__(self, context: Context) -> None:
		self.context = context

	def get(self) -> Dependencies:
		"""Get all pinned requirements, one per occurrence, in discovery order."""

		dependencies = Dependencies()
		for _, _, lock in self._targets():
			for line in lock.read_text(encoding="utf-8").splitlines():
				match = _PIN.match(line)
				if match is not None:
					dependencies.add(match.group(1), match.group(2), lock)
		return dependencies

	def upgrade(self, dependencies: Dependencies) -> None:
		"""Upgrade every requirements lock by running the update target of its compile rule."""

		targets = list(self._targets())
		snapshots = {lock: lock.read_bytes() for _, _, lock in targets}
		try:
			for label, root, _ in targets:
				print(".", end="", flush=True)
				self._runBazel(root, label)
		except BaseException:
			for path, content in snapshots.items():
				path.write_bytes(content)
			raise
		finally:
			# rules_python replaces os.replace with shutil.copy, which leaves pip-compile atomic writes behind.
			for _, _, lock in targets:
				for path in lock.parent.glob(".__atomic-write*"):
					path.unlink(missing_ok=True)

	def _targets(self) -> typing.Iterator[typing.Tuple[str, pathlib.Path, pathlib.Path]]:
		"""Discover the update targets of the compile_pip_requirements rules."""

		repos = self.context.getLocalBzlmod()
		expression = " + ".join(["//...", *(f"@{repo}//..." for repo in repos)])
		result = self.context.localBazel(
			["query", f'kind(py_binary, attr(generator_function, "pip_compile", {expression}))', "--output=location"],
			timeoutS=None,
		)
		for line in sorted(result.getStdout().splitlines()):
			location = _LOCATION.match(line)
			if location is None:
				continue
			path, label = location.groups()
			name = label.rsplit(":", 1)[-1].removesuffix(".update")
			lock = (pathlib.Path(path).parent / f"{name}.txt").resolve()
			root = lock.parent
			while not (root / "MODULE.bazel").exists():
				if root.parent == root:
					raise RuntimeError(f"Cannot find the module root of '{lock}'.")
				root = root.parent
			yield label, root, lock

	def _runBazel(self, moduleRoot: pathlib.Path, label: str) -> None:
		"""Run the update target from the module root so rules_python writes to the right files."""

		self.context.localBazel(
			[
				"run",
				f"--run_under=env -C {shlex.quote(str(moduleRoot))} BUILD_WORKSPACE_DIRECTORY={shlex.quote(str(moduleRoot))}",
				label,
				"--",
				"--upgrade",
			],
			timeoutS=None,
		)

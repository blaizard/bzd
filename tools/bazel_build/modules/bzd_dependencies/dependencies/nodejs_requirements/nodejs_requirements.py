import json
import pathlib
import re
import typing

from context.context import Context
from dependencies.interface import Dependencies

_LOCATION = re.compile(r"^(.*):\d+:\d+$")
_OUTPUT = re.compile(r"^\$\(rootpath .*:([^:]+)\)$")


def _parsePackage(package: str) -> tuple[str, str]:
	"""Split a package entry of a requirements file into a name and a version."""

	reference = package.split("@npm:", maxsplit=1)[-1]
	name, _, version = reference.rpartition("@")
	return name, version


class NodejsRequirements:
	"""Discover and upgrade the locks managed by bzd_nodejs_requirements_compile rules."""

	def __init__(self, context: Context) -> None:
		self.context = context

	def get(self) -> Dependencies:
		"""Get all pinned packages, one per occurrence, in discovery order."""

		dependencies = Dependencies()
		for _, lock in self._targets():
			content = json.loads(lock.read_text(encoding="utf-8"))
			for package in content["packages"]:
				name, version = _parsePackage(package)
				dependencies.add(name, version, lock)
		return dependencies

	def upgrade(self, dependencies: Dependencies) -> None:
		"""Upgrade every requirements lock by running its compile target."""

		targets = list(self._targets())
		snapshots = {lock: lock.read_bytes() for _, lock in targets}
		try:
			for label, _ in targets:
				print(".", end="", flush=True)
				self.context.localBazel(["run", label], timeoutS=None)
		except BaseException:
			for path, content in snapshots.items():
				path.write_bytes(content)
			raise

	def _targets(self) -> typing.Iterator[tuple[str, pathlib.Path]]:
		"""Discover the compile targets and their requirement locks."""

		repos = self.context.getLocalBzlmod()
		expression = " + ".join(["//...", *(f"@{repo}//..." for repo in repos)])
		result = self.context.localBazel(
			[
				"query",
				f'kind(py_binary, attr(generator_function, "bzd_nodejs_requirements_compile", {expression}))',
				"--output=streamed_jsonproto",
			],
			timeoutS=None,
		)
		targets = []
		for line in result.getStdout().splitlines():
			if not line.strip():
				continue
			entry = json.loads(line)
			if entry.get("type") != "RULE":
				continue
			rule = entry["rule"]
			attributes = {attribute["name"]: attribute for attribute in rule["attribute"]}
			args = attributes.get("args", {}).get("stringListValue", [])
			try:
				output = _OUTPUT.match(args[args.index("--output") + 1])
			except (ValueError, IndexError):
				output = None
			location = _LOCATION.match(rule["location"])
			if output is None or location is None:
				raise RuntimeError(f"Cannot find the requirements output of '{rule['name']}'.")
			targets.append((rule["name"], (pathlib.Path(location.group(1)).parent / output.group(1)).resolve()))
		yield from sorted(targets)

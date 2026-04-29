import argparse
import json
import os
import pathlib
import tarfile
import typing

virtualStore = ".store"


def encodeKey(key: str) -> str:
	"""Encode a key for use as a filesystem component."""
	return key.replace("/", "+")


def nameFromKey(key: str) -> str:
	"""Get the package name from the key."""
	return key.rsplit("@", 1)[0]


def canonicalFromKey(key: str) -> str:
	"""Get the canonical form of a key (handles aliases)."""
	if "@npm:" in key:
		return key.split("@npm:")[0]
	return key


def makeSymlink(linkPath: pathlib.Path, linkTarget: str) -> None:
	"""Create a symlink."""
	linkPath.parent.mkdir(parents=True, exist_ok=True)
	# TODO: understand why this is needed and add an assertion instead.
	linkPath.unlink(missing_ok=True)
	linkPath.symlink_to(linkTarget)


def extractTar(src: pathlib.Path, dest: pathlib.Path) -> None:
	"""Extract a package tar archive."""

	def stripTopLevel(member: tarfile.TarInfo) -> typing.Optional[tarfile.TarInfo]:
		_, _, rest = member.name.partition("/")
		if not rest:
			return None
		member.name = rest
		return member

	with tarfile.open(src, "r") as tar:
		tar.extractall(dest, members=(s for m in tar.getmembers() if (s := stripTopLevel(m)) is not None), filter="data")


def installVirtualStore(nodeModules: pathlib.Path, tarByKey: typing.Dict[str, pathlib.Path]) -> None:
	"""Extract all packages tarball into the virtual store."""

	for key, tarPath in tarByKey.items():
		name = nameFromKey(key)
		encodedKey = encodeKey(key)
		pkgStorePath = nodeModules / virtualStore / encodedKey / "node_modules" / name
		if pkgStorePath.exists():
			continue
		pkgStorePath.mkdir(parents=True)
		extractTar(tarPath, pkgStorePath)


def installDepSymlinks(
	nodeModules: pathlib.Path, tarByKey: typing.Dict[str, pathlib.Path], graph: typing.Dict[str, typing.List[str]]
) -> None:
	"""Create dependency symlinks inside each package's virtual store entry."""

	for key, deps in graph.items():
		encodedKey = encodeKey(key)
		for depKey in deps:
			assert depKey in tarByKey, f"The dependency '{depKey}' for '{key}' is missing."
			encodedDepKey = encodeKey(depKey)
			depName = nameFromKey(depKey)
			linkPath = nodeModules / virtualStore / encodedKey / "node_modules" / depName
			linkTarget = os.path.relpath(nodeModules / virtualStore / encodedDepKey / "node_modules" / depName, linkPath.parent)
			makeSymlink(linkPath, linkTarget)


def installRootPackages(
	nodeModules: pathlib.Path,
	tarByKey: typing.Dict[str, pathlib.Path],
	graph: typing.Dict[str, typing.List[str]],
	rootKeys: typing.Set[str],
) -> None:
	"""Hoist all transitive dependencies to top-level node_modules via BFS from root Keys."""

	hoisted: typing.Dict[str, str] = {}
	bfsQueue = [k for k in rootKeys if k in tarByKey]
	bfsVisited: typing.Set[str] = set(bfsQueue)
	for key in bfsQueue:
		name = nameFromKey(key)
		if name not in hoisted:
			hoisted[name] = key
		for depKey in graph.get(key, []):
			if depKey not in bfsVisited:
				assert depKey in tarByKey, f"The dependency '{depKey}' for '{key}' is missing."
				bfsVisited.add(depKey)
				bfsQueue.append(depKey)

	for name, key in hoisted.items():
		encodedKey = encodeKey(key)
		linkPath = nodeModules / name
		linkTarget = os.path.relpath(nodeModules / virtualStore / encodedKey / "node_modules" / name, linkPath.parent)
		makeSymlink(linkPath, linkTarget)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Generate a node_modules directory.")
	parser.add_argument(
		"--package", nargs=2, action="append", metavar=("PACKAGE", "TAR"), default=[], help="A package entry."
	)
	parser.add_argument(
		"--dependency",
		type=pathlib.Path,
		default=[],
		action="append",
		help="Path to a per-package dependency tree JSON file.",
	)
	parser.add_argument("node_modules", type=pathlib.Path, help="The path of the node_modules directory to be set-up")
	args = parser.parse_args()

	tarByKey = {canonicalFromKey(key): pathlib.Path(path) for key, path in args.package}

	graph: typing.Dict[str, typing.List[str]] = {}
	rootKeys: typing.Set[str] = set()
	for depFile in args.dependency:
		data = json.loads(depFile.read_text())
		rootKeys.add(canonicalFromKey(data["package"]))
		for k, v in data.get("packages", {}).items():
			normalizedKey = canonicalFromKey(k)
			normalizedDeps = [canonicalFromKey(d) for d in v.get("dependencies", [])]
			graph.setdefault(normalizedKey, normalizedDeps)

	installVirtualStore(args.node_modules, tarByKey)
	installDepSymlinks(args.node_modules, tarByKey, graph)
	installRootPackages(args.node_modules, tarByKey, graph, rootKeys)

import argparse
import json
import pathlib
import tempfile
import subprocess
import os
import typing

_compressionToArgs = {
	# Required for reproducibility, as gzip by default adds a timestamp to the header which makes the output non deterministic.
	"gz": ["--gzip", "--options=gzip:!timestamp"],
	"bz2": ["--bzip2"],
	"xz": ["--xz"],
}


class Mtree:
	def __init__(self) -> None:
		self.mtree = ["#mtree", "/set uid=0 gid=0 time=0"]

	def addDirectory(self, path: pathlib.Path) -> None:
		self.mtree.append(f"{path} mode=0755 type=dir")

	def addFile(self, path: pathlib.Path) -> None:
		if os.access(path, os.X_OK):
			self.mtree.append(f"{path} mode=0755 type=file")
		else:
			self.mtree.append(f"{path} mode=0644 type=file")

	def addLink(self, path: pathlib.Path, target: pathlib.Path) -> None:
		self.mtree.append(f"{path} mode=0777 type=link link={target}")

	def toBytes(self) -> bytes:
		return "\n".join(self.mtree).encode() + b"\n"


class TreeBuilder:
	def __init__(self) -> None:
		self.mtree: typing.Optional[Mtree] = None
		self.entries: typing.Dict[pathlib.Path, str] = {}
		self.files: typing.Dict[pathlib.Path, typing.Set[pathlib.Path]] = {}
		self.directories: typing.Dict[pathlib.Path, typing.Set[pathlib.Path]] = {}
		self.links: typing.Dict[pathlib.Path, pathlib.Path] = {}

	def _addEntry(self, path: pathlib.Path, data: typing.Any, check: bool = True) -> None:
		if check and path in self.entries:
			raise Exception(f"Duplicate entry for path '{path}', original entry: {self.entries[path]}, new entry: {data}")
		self.entries[path] = data

	def addFile(self, path: pathlib.Path) -> None:
		assert self.mtree is None, "Mtree already generated, cannot add new entry."
		actual = path.resolve()
		self.files.setdefault(actual, set()).add(path)
		self._addEntry(path, "file")

	def addDirectory(self, path: pathlib.Path) -> None:
		assert self.mtree is None, "Mtree already generated, cannot add new entry."
		actual = path.resolve()
		self.directories.setdefault(actual, set()).add(path)
		self._addEntry(path, "directory")

	def addLink(self, path: pathlib.Path, target: pathlib.Path, check: bool = True) -> None:
		assert self.mtree is None, "Mtree already generated, cannot add new entry."
		assert not target.is_absolute(), f"Link target '{target}' must be a relative path."

		self.links[path] = target
		self._addEntry(path, f"link -> {target}", check=check)

	def addAbsoluteLink(self, path: pathlib.Path, targetPath: pathlib.Path, check: bool = True) -> None:
		target = targetPath.relative_to(path.parent, walk_up=True)
		self.addLink(path, target, check=check)

	@staticmethod
	def _identifyPathAndSymlinks(
		paths: typing.List[pathlib.Path],
	) -> typing.Tuple[pathlib.Path, typing.List[pathlib.Path]]:
		"""Given a list of paths, identify the actual path and the symlinks pointing to it."""
		if len(paths) == 1:
			return paths[0], []
		paths.sort(key=lambda p: (len(p.parts), str(p)))
		# Look for the first non-symlink path, this is the actual file/directory. The rest are symlinks pointing to it.
		for path in paths:
			if not path.is_symlink():
				return path, [p for p in paths if p != path]
		return paths[0], paths[1:]

	@staticmethod
	def _listDirectory(
		root: pathlib.Path,
	) -> typing.Iterator[typing.Tuple[typing.List[pathlib.Path], typing.List[pathlib.Path], typing.List[pathlib.Path]]]:
		"""List the content of a directory, including and preserving symlinks.

		The paths returned are relative to the root.
		Preserving symlinks within a declared bazel directory makes sense as bazel ensures that symlinks does not escape the declared directory.
		"""

		for dirPath, dirNames, fileNames in os.walk(root, topdown=True, followlinks=False):
			# Make the traversal deterministic by sorting the directory and file names.
			dirNames.sort()
			fileNames.sort()

			relativePath = pathlib.Path(dirPath).relative_to(root)
			symlinks: typing.List[pathlib.Path] = []
			directories: typing.List[pathlib.Path] = []
			files: typing.List[pathlib.Path] = []
			for name in dirNames:
				current = relativePath / name
				(symlinks if (root / current).is_symlink() else directories).append(current)
			for name in fileNames:
				current = relativePath / name
				(symlinks if (root / current).is_symlink() else files).append(current)
			yield directories, files, symlinks

	def generate(self) -> bytes:
		"""Generate the mtree content, this is a deterministic representation of the file system structure."""

		if self.mtree is None:
			mtree = Mtree()

			# Set the directory entries, this ensure that parent directories are created before their content.
			for actual, paths in self.directories.items():
				path, symlinks = self._identifyPathAndSymlinks(list(paths))

				mtree.addDirectory(path)
				for directoryPaths, filePaths, linkPaths in self._listDirectory(actual):
					for relative in directoryPaths:
						mtree.addDirectory(path / relative)
					for relative in filePaths:
						self.addFile(path / relative)
					for relative in linkPaths:
						target = (actual / relative).readlink()
						if target.is_absolute():
							self.addFile(path / relative)
						else:
							self.addLink(path / relative, target)

				for symlink in symlinks:
					self.addAbsoluteLink(symlink, path, check=False)

			# Set the file entries.
			for actual, paths in self.files.items():
				path, symlinks = self._identifyPathAndSymlinks(list(paths))
				mtree.addFile(path)
				for symlink in symlinks:
					self.addAbsoluteLink(symlink, path, check=False)

			# Set the link entries.
			for path, target in self.links.items():
				mtree.addLink(path, target)

			self.mtree = mtree

		return self.mtree.toBytes()


def normalize(path: typing.Union[str, pathlib.Path]) -> pathlib.Path:
	"""Normalize a path by removing redundant parts."""

	return pathlib.Path(os.path.normpath(path))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Tar bundler.")
	parser.add_argument(
		"--output",
		default=pathlib.Path("/tmp/output"),
		type=pathlib.Path,
		help="The output path where the bundle should be created.",
	)
	parser.add_argument(
		"--tar",
		required=True,
		type=pathlib.Path,
		help="The path of the bsdtar executable.",
	)
	parser.add_argument(
		"--compression",
		choices=["gz", "bz2", "xz"],
		help="The type of compression for the tarball.",
	)
	parser.add_argument(
		"manifest",
		type=pathlib.Path,
		help="The path of the manifest file describing the content of the bundle.",
	)
	args = parser.parse_args()

	manifest = json.loads(args.manifest.read_text())
	builder = TreeBuilder()

	# Process the files and directories, this will also populate the mtree with the content of the directories.
	for path in manifest["files"]:
		path = normalize(path)
		if path.is_file():
			builder.addFile(path)
		elif path.is_dir():
			builder.addDirectory(path)
		else:
			raise Exception(f"Path '{path}' is neither a file nor a directory.")

	# Process the symlinks, this will add the link entries to the mtree.
	for path, info in manifest["symlinks"].items():
		symlink = normalize(path)
		target = normalize(info["target"])
		if info["type"] == "relative":
			builder.addLink(symlink, target)
		elif info["type"] == "absolute":
			builder.addAbsoluteLink(symlink, target)
		else:
			raise Exception(f"Unsupported symlink type '{info['type']}' for path '{path}'.")

	extraArguments = []
	if args.compression:
		extraArguments.extend(_compressionToArgs[args.compression])

	with tempfile.NamedTemporaryFile() as mtreeFile:
		mtreeFile.write(builder.generate())
		mtreeFile.flush()
		subprocess.run(
			[args.tar, "-cf", args.output, *extraArguments, "--dereference", "--format=pax", f"@{mtreeFile.name}"],
			check=True,
		)

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


class Layers:
	"""Represents the layers of the bundle, it keeps track of all layers and the files already added."""

	def __init__(self) -> None:
		self.all: typing.Set[pathlib.Path] = set([pathlib.Path()])
		self.current: typing.Set[pathlib.Path] = set([pathlib.Path()])

	def __contains__(self, path: pathlib.Path) -> bool:
		"""Check if a path is already written."""

		return path in self.all or path in self.current

	def inCurrentLayer(self, path: pathlib.Path) -> bool:
		"""Check if a path is already written in the current layer."""

		return path in self.current

	def newLayer(self) -> None:
		"""Create a new layer."""

		self.all.update(self.current)
		self.current = set([pathlib.Path()])

	def add(self, path: pathlib.Path) -> None:
		"""Add a path to the current layer."""

		self.current.add(path)


class Mtree:
	"""Create an mtree representation of the file system structure, this is used to create a deterministic tarball with bsdtar."""

	def __init__(self, layers: Layers) -> None:
		self.mtree = ["#mtree", "/set uid=0 gid=0 time=0"]
		self.layers = layers

	def addParentDirectories(self, parent: pathlib.Path) -> None:
		"""Add parent directories to a current path."""

		while parent != pathlib.Path(".") and not self.layers.inCurrentLayer(parent):
			self.mtree.append(f"./{parent} mode=0755 type=dir")
			self.layers.add(parent)
			parent = parent.parent

	def addDirectory(self, path: pathlib.Path) -> bool:
		"""Add a directory to the mtree."""

		if path in self.layers:
			return False
		self.addParentDirectories(path.parent)
		self.mtree.append(f"./{path} mode=0755 type=dir")
		self.layers.add(path)
		return True

	def addFile(self, path: pathlib.Path) -> None:
		"""Add a file to the mtree."""

		if path in self.layers:
			return
		self.addParentDirectories(path.parent)
		if os.access(path, os.X_OK):
			self.mtree.append(f"./{path} mode=0755 type=file")
		else:
			self.mtree.append(f"./{path} mode=0644 type=file")
		self.layers.add(path)

	def addLink(self, path: pathlib.Path, target: pathlib.Path) -> None:
		"""Add a symlink to the mtree."""

		if path in self.layers:
			return
		self.addParentDirectories(path.parent)
		self.mtree.append(f"./{path} mode=0777 type=link link={target}")
		self.layers.add(path)

	def toBytes(self) -> bytes:
		return "\n".join(self.mtree).encode() + b"\n"


class PathCollection:
	"""Represents a collection of paths, this is used to identify which file/directory should be copied and which should be symlinked."""

	def __init__(self) -> None:
		self.paths: typing.Set[pathlib.Path] = set()
		self.path: typing.Optional[pathlib.Path] = None

	def add(self, path: pathlib.Path) -> None:
		self.paths.add(path)

	def identifyPathAndSymlinks(self) -> typing.Tuple[pathlib.Path, typing.List[pathlib.Path]]:
		"""Identify the actual path and the symlinks pointing to it."""

		if self.path is None:
			self.path = self._identifyPath(list(self.paths))
		return self.path, sorted([p for p in self.paths if p != self.path])

	@staticmethod
	def _identifyPath(paths: typing.List[pathlib.Path]) -> pathlib.Path:
		"""Given a list of paths, identify the actual path to be copied."""
		if len(paths) != 1:
			paths.sort(key=lambda p: (len(p.parts), str(p)))
			# Look for the first non-symlink path, this is the actual file/directory. The rest are symlinks pointing to it.
			for path in paths:
				if not path.is_symlink():
					return path
		return paths[0]


class TreeBuilder:
	def __init__(self) -> None:
		self.mtree: typing.Optional[Mtree] = None
		self.entries: typing.Dict[pathlib.Path, str] = {}
		self.files: typing.Dict[typing.Union[typing.Tuple[int, int], pathlib.Path], PathCollection] = {}
		self.directories: typing.Dict[typing.Union[typing.Tuple[int, int], pathlib.Path], PathCollection] = {}
		self.links: typing.Dict[pathlib.Path, pathlib.Path] = {}

	def _addEntry(self, path: pathlib.Path, data: typing.Any) -> None:
		if path in self.entries and self.entries[path] != data:
			raise Exception(
				f"Duplicate entry for path '{path}' with different type, original entry: {self.entries[path]}, new entry: {data}"
			)
		self.entries[path] = data

	def addFile(self, path: pathlib.Path) -> None:
		assert self.mtree is None, "Mtree already generated, cannot add new entry."
		self.files.setdefault(self._fileKey(path), PathCollection()).add(path)
		self._addEntry(path, "file")

	def addDirectory(self, path: pathlib.Path) -> None:
		assert self.mtree is None, "Mtree already generated, cannot add new entry."
		self.directories.setdefault(self._fileKey(path), PathCollection()).add(path)
		self._addEntry(path, "directory")

	def addLink(self, path: pathlib.Path, target: pathlib.Path) -> None:
		assert self.mtree is None, "Mtree already generated, cannot add new entry."
		assert not target.is_absolute(), f"Link target '{target}' must be a relative path."

		self.links[path] = target
		self._addEntry(path, f"link -> {target}")

	def addAbsoluteLink(self, path: pathlib.Path, targetPath: pathlib.Path) -> None:
		target = self._linkFromPath(path, targetPath)
		self.addLink(path, target)

	@staticmethod
	def _fileKey(path: pathlib.Path) -> typing.Union[typing.Tuple[int, int], pathlib.Path]:
		"""Return a hashable key that uniquely identifies the underlying file."""

		stat = path.stat()
		# When st_ino == 0, inodes are not meaningful (for FAT/exFAT for example).
		if stat.st_ino == 0:
			return path.resolve()
		return (stat.st_dev, stat.st_ino)

	@staticmethod
	def _linkFromPath(path: pathlib.Path, targetPath: pathlib.Path) -> pathlib.Path:
		"""Get the relative link target from a path, this is used to identify the target of a symlink."""

		return targetPath.relative_to(path.parent, walk_up=True)

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

	def generate(self, mtree: Mtree) -> bytes:
		"""Generate the mtree content, this is a deterministic representation of the file system structure."""

		# Set the directory entries, this ensure that parent directories are created before their content.
		for collection in self.directories.values():
			path, symlinks = collection.identifyPathAndSymlinks()

			if mtree.addDirectory(path):
				for directoryPaths, filePaths, linkPaths in self._listDirectory(path):
					for relative in directoryPaths:
						mtree.addDirectory(path / relative)
					for relative in filePaths:
						self.addFile(path / relative)
					for relative in linkPaths:
						target = (path / relative).readlink()
						if target.is_absolute():
							self.addFile(path / relative)
						else:
							self.addLink(path / relative, target)

			for symlink in symlinks:
				target = self._linkFromPath(symlink, path)
				mtree.addLink(symlink, target)

		# Set the file entries.
		for collection in self.files.values():
			path, symlinks = collection.identifyPathAndSymlinks()
			mtree.addFile(path)
			for symlink in symlinks:
				target = self._linkFromPath(symlink, path)
				mtree.addLink(symlink, target)

		# Set the link entries.
		for symlink, target in self.links.items():
			mtree.addLink(symlink, target)

		return mtree.toBytes()


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
	layers = Layers()

	extraArguments = []
	if args.compression:
		extraArguments.extend(_compressionToArgs[args.compression])

	for layer in manifest["layers"]:
		layers.newLayer()

		# Process the files and directories, this will also populate the mtree with the content of the directories.
		for path in layer["files"]:
			path = normalize(path)
			if path.is_file():
				builder.addFile(path)
			elif path.is_dir():
				builder.addDirectory(path)
			else:
				raise Exception(f"Path '{path}' is neither a file nor a directory.")

		# Process the symlinks, this will add the link entries to the mtree.
		for path, info in layer["symlinks"].items():
			symlink = normalize(path)
			target = normalize(info["target"])
			if info["type"] == "relative":
				builder.addLink(symlink, target)
			elif info["type"] == "absolute":
				builder.addAbsoluteLink(symlink, target)
			else:
				raise Exception(f"Unsupported symlink type '{info['type']}' for path '{path}'.")

		with tempfile.NamedTemporaryFile() as mtreeFile:
			mtreeBytes = builder.generate(Mtree(layers))
			mtreeFile.write(mtreeBytes)
			mtreeFile.flush()
			subprocess.run(
				[
					args.tar,
					"-cf",
					args.output / layer["output"],
					*extraArguments,
					"--dereference",
					"--format=pax",
					f"@{mtreeFile.name}",
				],
				check=True,
			)

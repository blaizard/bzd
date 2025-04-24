import argparse
import pathlib
import hashlib
import io
import tarfile
import tempfile
import typing
from unittest.mock import patch
import sys

class Bundler:

	def __init__(
	        self, executables: typing.List[typing.Tuple[pathlib.Path, pathlib.Path, pathlib.Path,
	                                                    pathlib.Path]]) -> None:
		self.executables = executables or []

	@staticmethod
	def makeTarInfoFromPath(path: pathlib.Path, arcname: pathlib.Path) -> tarfile.TarInfo:
		tarinfo = tarfile.TarInfo(name=arcname.as_posix())
		stat = path.stat()
		isExecutable = (stat.st_mode & 0o111) != 0
		tarinfo.mode = 0o755 if isExecutable else 0o644
		tarinfo.size = stat.st_size
		tarinfo.mtime = 0
		tarinfo.uid = 0
		tarinfo.gid = 0
		return tarinfo

	@staticmethod
	def makeTarInfoFromContentExecutable(content: bytes, arcname: pathlib.Path) -> tarfile.TarInfo:
		tarinfo = tarfile.TarInfo(name=arcname.as_posix())
		tarinfo.mode = 0o755
		tarinfo.size = len(content)
		tarinfo.mtime = 0
		tarinfo.uid = 0
		tarinfo.gid = 0
		return tarinfo

	@staticmethod
	def makeTarInfoForSymlink(symlink: pathlib.Path, arcname: pathlib.Path) -> tarfile.TarInfo:
		tarinfo = tarfile.TarInfo(name=arcname.as_posix())
		tarinfo.mode = 0o777
		tarinfo.type = tarfile.SYMTYPE
		tarinfo.linkname = symlink.as_posix()
		tarinfo.size = 0
		tarinfo.mtime = 0
		tarinfo.uid = 0
		tarinfo.gid = 0
		return tarinfo

	@staticmethod
	def toAbsolute(path: pathlib.Path, root: str) -> str:
		"""Make a path into an absolute path in not already."""
		if path.is_absolute():
			return path.as_posix()
		return f"{root}/{path.as_posix()}"

	@staticmethod
	def fileHash(path: pathlib.Path) -> str:
		"""Calculate the hash of a file."""
		hasher = hashlib.new("md5")
		size = 0
		with path.open("rb") as file:
			while True:
				chunk = file.read(1024 * 1024)
				if not chunk:
					break
				hasher.update(chunk)
				size += len(chunk)
		return f"{hasher.hexdigest()}.{size}"

	def process(self, output: pathlib.Path, compression: typing.Optional[str],
	            bootstrapScript: typing.Optional[pathlib.Path]) -> None:

		mode = "w"
		if compression:
			mode += ":" + compression

		# Mock the current time, this is needed because we don't have control on what compression algorithm do.
		# In this case with the 'gzip' algorithm, it will add the modification time to the archive.
		with patch("time.time", return_value=0.0):
			with tarfile.open(output, mode=mode) as tar:

				files: typing.Dict[str, typing.Dict[str, typing.Any]] = {}

				# Sort by prefix, this ensures reproducibility.
				for manifest, prefix, entryPoint, workspace in sorted(self.executables, key=lambda x: x[1]):
					# Gather all the files.
					atLeastOneEntry = False
					for line in sorted(manifest.read_text().split("\n")):
						split = list(filter(str.strip, line.split()))
						if len(split) == 0:
							continue
						elif len(split) <= 2:
							target = (prefix / split[0]) if prefix else pathlib.Path(split[0])
							path = (manifest.parent / split[0]).resolve()
						else:
							raise RuntimeError(f"Manifest '{manifest}' has an unexpected format.")
						if path.is_file():
							fileHash = Bundler.fileHash(path)
							if fileHash not in files:
								files[fileHash] = {"path": path, "targets": []}
							# TODO: handle hash collisions.
							else:
								pass

							files[fileHash]["targets"].append(target)
							atLeastOneEntry = True

					assert atLeastOneEntry, f"There are no file referenced in the manifest: {manifest}, content:\n{manifest.read_text()[:100]}[...]"

					# Adds a bootstrap executable.
					if bootstrapScript:
						bootstrap = """#!/usr/bin/env bash
set -e

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd "{workingDirectory}"
export RUNFILES_DIR="{runfilesDirectory}"
exec "{executable}" "$@"
""".format(workingDirectory=Bundler.toAbsolute(prefix / workspace, "$SCRIPTPATH"),
						runfilesDirectory=Bundler.toAbsolute(prefix, "$SCRIPTPATH"),
						executable=entryPoint.as_posix()).encode("utf-8")
						tarinfo = Bundler.makeTarInfoFromContentExecutable(bootstrap, prefix / bootstrapScript)
						file = io.BytesIO(bootstrap)
						tar.addfile(tarinfo, fileobj=file)

				# Copy all the files and create symlinks for the duplicates.
				# Note, that we sort the array before iterating to ensure reproducibility.
				for fileHash, data in sorted(files.items(), key=lambda x: x[0]):
					target, *symlinks = data["targets"]
					print(fileHash, data["targets"])

					# Update the file information within the tarball that ensures reproducibility.
					tarinfo = Bundler.makeTarInfoFromPath(data["path"], target)
					with data["path"].open("rb") as fd:
						tar.addfile(tarinfo, fileobj=fd)

					for symlink in symlinks:
						relativePath = pathlib.Path(target).relative_to(pathlib.Path(symlink).parent, walk_up=True)
						tarinfo = Bundler.makeTarInfoForSymlink(relativePath, symlink)
						tar.addfile(tarinfo, fileobj=None)

		sys.exit(1)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bundler.")
	parser.add_argument(
	    "--output",
	    default=pathlib.Path("/tmp/output.tar"),
	    type=pathlib.Path,
	    help="The output path for the archive.",
	)
	parser.add_argument(
	    "--compression",
	    choices=["gz", "bz2", "xz"],
	    help="The type of compression for the tarball.",
	)
	parser.add_argument(
	    "--bootstrap-script",
	    type=pathlib.Path,
	    help="Create a bootstrap script for all executables.",
	)
	parser.add_argument("--executable",
	                    dest="executables",
	                    nargs=4,
	                    action="append",
	                    type=pathlib.Path,
	                    metavar=("manifest", "prefix", "entry_point", "workspace"),
	                    help="Description of an executable to bundle.")

	args = parser.parse_args()

	bundler = Bundler(executables=args.executables)
	bundler.process(output=args.output, compression=args.compression, bootstrapScript=args.bootstrap_script)

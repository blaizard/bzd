import argparse
import pathlib
import shutil
import tarfile
import tempfile


def resetTarInfo(info: tarfile.TarInfo) -> tarfile.TarInfo:
	"""Zero out non-deterministic metadata."""

	info.mtime = 0
	info.uid = 0
	info.gid = 0
	info.uname = ""
	info.gname = ""
	info.pax_headers = {}

	# Normalize permissions
	if info.isdir():
		info.mode = 0o755
	elif info.issym() or info.islnk():
		info.mode = 0o777
	else:
		info.mode = 0o644

	return info


def createTar(source: pathlib.Path, destination: pathlib.Path) -> None:
	"""Create a deterministic package archive from a source directory."""

	destination.parent.mkdir(parents=True, exist_ok=True)
	with tarfile.open(destination, "w", format=tarfile.GNU_FORMAT) as tar:
		# Sorted walk makes it deterministic.
		allPaths = set(source.rglob("*")) | set(source.rglob(".*"))
		for path in sorted(allPaths, key=lambda p: p.as_posix()):
			arcname = "./" + path.relative_to(source).as_posix()
			tar.add(path, arcname=arcname, recursive=False, filter=resetTarInfo)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Repackage a npm package into a deterministic archive.")
	parser.add_argument(
		"--output",
		type=pathlib.Path,
		required=True,
		help="The destination path where to write the repackaged archive.",
	)
	parser.add_argument(
		"srcs",
		type=pathlib.Path,
		help="The source directory to be included in the package.",
	)
	args = parser.parse_args()

	with tempfile.TemporaryDirectory() as tempDir:
		tempDirPath = pathlib.Path(tempDir)
		# Copytree dereferences the symlinks created by Bazel's directory artifact.
		shutil.copytree(args.srcs, tempDirPath / "pkg")
		createTar(source=tempDirPath / "pkg", destination=args.output)

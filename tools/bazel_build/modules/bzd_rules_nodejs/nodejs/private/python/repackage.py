import argparse
import os
import pathlib
import tarfile
import tempfile
import shutil
import stat


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
		# Mask against owner execute bit only (0o100)
		# Bazel tracks only this bit the rest of the executable bits are OS dependent/noise.
		if info.mode & stat.S_IXUSR:
			info.mode = 0o755
		else:
			info.mode = 0o644

	return info


def createTar(source: pathlib.Path, destination: pathlib.Path) -> None:
	"""Create a package tar archive."""

	destination.parent.mkdir(parents=True, exist_ok=True)
	with tarfile.open(destination, "w", format=tarfile.GNU_FORMAT) as tar:
		# Sorted walk make it deterministic.
		allPaths = set(source.rglob("*")) | set(source.rglob(".*"))
		for path in sorted(allPaths, key=lambda p: p.as_posix()):
			arcname = "./" + path.relative_to(source).as_posix()
			tar.add(path, arcname=arcname, recursive=False, filter=resetTarInfo)


def createSymlink(linkPath: pathlib.Path, relativePath: str) -> None:
	"""Create a symlink at linkPath pointing to a relative link."""

	linkPath.parent.mkdir(parents=True, exist_ok=True)
	os.symlink(relativePath, linkPath)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		description="Repackage a npm package tgz into an archive and includes relative symlinks for its dependencies."
	)
	parser.add_argument(
		"--output",
		type=pathlib.Path,
		required=True,
		help="The destination path where to write the repackaged archive.",
	)
	parser.add_argument(
		"--root",
		type=pathlib.Path,
		required=True,
		help="The root directory.",
	)
	parser.add_argument(
		"--symlink",
		metavar=("NAME", "RELATIVE_PATH"),
		nargs=2,
		action="append",
		default=[],
		help="Relative symlink to be included in the package. Can be used multiple times to include multiple symlinks.",
	)
	parser.add_argument(
		"srcs",
		type=pathlib.Path,
		help="The source directory to be included in the package.",
	)
	args = parser.parse_args()

	with tempfile.TemporaryDirectory() as tempDir:
		tempDirPath = pathlib.Path(tempDir)
		shutil.copytree(args.srcs, tempDirPath / args.root)
		for moduleName, relativePath in args.symlink:
			createSymlink(linkPath=tempDirPath / args.root / "node_modules" / moduleName, relativePath=relativePath)
		createTar(source=tempDirPath, destination=args.output)

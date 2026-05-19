import argparse
import os
import pathlib
import tarfile
import tempfile
import typing


def extractTar(source: pathlib.Path, destination: pathlib.Path) -> None:
	"""Extract a package tar archive."""

	def stripTopLevel(member: tarfile.TarInfo) -> typing.Optional[tarfile.TarInfo]:
		_, _, rest = member.name.partition("/")
		if not rest:
			return None
		member.name = rest
		return member

	with tarfile.open(source, "r") as tar:
		tar.extractall(
			destination, members=(s for m in tar.getmembers() if (s := stripTopLevel(m)) is not None), filter="data"
		)


def createTar(source: pathlib.Path, destination: pathlib.Path) -> None:
	"""Create a package tar archive."""

	destination.parent.mkdir(parents=True, exist_ok=True)
	with tarfile.open(destination, "w") as tar:
		tar.add(source, arcname=".", recursive=True)


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
		"archive",
		type=pathlib.Path,
		help="The path of the package archive to be extracted.",
	)
	args = parser.parse_args()

	with tempfile.TemporaryDirectory() as tempDir:
		tempDirPath = pathlib.Path(tempDir)
		extractTar(source=args.archive, destination=tempDirPath / args.root)
		for moduleName, relativePath in args.symlink:
			createSymlink(linkPath=tempDirPath / args.root / "node_modules" / moduleName, relativePath=relativePath)
		createTar(source=tempDirPath, destination=args.output)

import argparse
import pathlib
import tarfile
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


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Extract a package tar archive to the given path.")
	parser.add_argument(
		"--output",
		type=pathlib.Path,
		help="The destination path where to extract the archive.",
	)
	parser.add_argument(
		"archive",
		type=pathlib.Path,
		help="The path of the package archive to be extracted.",
	)
	args = parser.parse_args()

	extractTar(source=args.archive, destination=args.output)

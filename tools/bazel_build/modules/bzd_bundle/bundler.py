import argparse
import pathlib
import tarfile
import tempfile
import stat


class Bundler:

	def __init__(self, path: pathlib.Path, executable: pathlib.Path, cwd: pathlib.Path) -> None:
		self.path = path
		self.executable = executable
		self.cwd = cwd

	def process(self, output: pathlib.Path) -> None:
		with tempfile.TemporaryFile(suffix=".tar.gz") as fd:
			# Create an archive with the files from the manifest.
			with tarfile.open(fileobj=fd, mode="w:gz") as tar:
				for line in self.path.read_text().split("\n"):
					splited = line.split()
					# Empty line
					if len(splited) < 2:
						continue
					elif len(splited) > 2:
						raise RuntimeError("Manifest seems to use white spaces which is not supported.")
					target, path = splited
					if pathlib.Path(path).is_file():
						tar.add(path, arcname=target)
			fd.flush()
			fd.seek(0)

			# Create the self extracting output file.
			with open(output, mode="wb") as fo:
				fo.write("""#!/bin/bash
set -e

case $1 in
	--extract)
		temp="$2"
		mkdir -p "$temp"
		shift
		shift
		;;
	*)
		temp=$(mktemp -d)
		;;
esac

temp=$(readlink -f "$temp")
if [[ ! "$temp" || ! -d "$temp" ]]; then
	echo "Could not create temporary directory."
	exit 1
fi

function cleanup {{
  rm -rf "$temp"
}}
trap cleanup EXIT

sed '0,/^#__END_OF_SCRIPT__#$/d' "$0" | tar -C "$temp" -zx
cd "$temp/{cwd}"
RUNFILES_DIR="$temp" "{executable}" "$@"
exit 0;

#__END_OF_SCRIPT__#
""".format(executable=str(self.executable), cwd=str(self.cwd)).encode("utf-8"))
				fo.write(fd.read())

		# Set permission to executable.
		output.chmod(output.stat().st_mode | stat.S_IEXEC)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bundler.")
	parser.add_argument(
	    "-o",
	    "--output",
	    default="/tmp/output.sh",
	    help="The output path for the self extracting bundle.",
	)
	parser.add_argument(
	    "-c",
	    "--cwd",
	    default=".",
	    help="The relative directory where the execution takes place.",
	)
	parser.add_argument("manifest", help="The path of the manifest to be used.")
	parser.add_argument("executable", help="The path of the executable within the archive.")

	args = parser.parse_args()

	bundler = Bundler(
	    pathlib.Path(args.manifest),
	    pathlib.Path(args.executable),
	    pathlib.Path(args.cwd),
	)
	bundler.process(pathlib.Path(args.output))

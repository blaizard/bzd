import argparse
import pathlib
import tarfile
import tempfile
import stat
import hashlib
import typing


class Bundler:

	def __init__(self, path: pathlib.Path, executable: pathlib.Path, cwd: pathlib.Path) -> None:
		self.path = path
		self.executable = executable
		self.cwd = cwd

	def process(self, output: pathlib.Path, args: typing.List[str]) -> None:

		with tempfile.TemporaryFile(suffix=".tar.gz") as fd:

			# Create an archive with the files from the manifest.
			with tarfile.open(fileobj=fd, mode="w:gz") as tar:
				# Find all the files.
				files: typing.Dict[pathlib.Path, typing.List[str]] = {}
				for line in self.path.read_text().split("\n"):
					split = list(filter(str.strip, line.split()))
					if len(split) == 0:
						continue
					elif len(split) <= 2:
						target = split[0]
						path = (self.path.parent / split[0]).resolve()
					else:
						raise RuntimeError("Manifest seems to use white spaces which is not supported.")
					if path.is_file():
						files.setdefault(path, [])
						files[path].append(target)

				assert len(
				    files.keys()
				) > 0, f"There are no file referenced in the manifest: {self.path}, content:\n{self.path.read_text()[:100]}[...]"

				# Copy all the files and create symlinks for the duplicates.
				for path, targets in files.items():
					target, *symlinks = targets
					tar.add(path, arcname=target)
					for symlink in symlinks:
						with tempfile.TemporaryDirectory() as tmp:
							relativePath = pathlib.Path(target).relative_to(pathlib.Path(symlink).parent, walk_up=True)
							symlinkPath = pathlib.Path(tmp) / "symlink"
							symlinkPath.symlink_to(relativePath)
							tar.add(symlinkPath, arcname=symlink)

			fd.flush()
			fd.seek(0)

			# Compute the checksum, this ensure reproductibility over builds.
			stamp = hashlib.md5(fd.read()).hexdigest()
			fd.seek(0)

			# Create the self extracting output file.
			with open(output, mode="wb") as fileOut:
				fileOut.write("""#!/usr/bin/env bash
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

if [[ ! -f "$temp/{stamp}" ]]; then
	function cleanup {{
		rm -rf "$temp"
	}}
	trap cleanup EXIT

	sed '0,/^#__END_OF_SCRIPT__#$/d' "$0" | tar -C "$temp" -zx
	touch "$temp/{stamp}"
fi

export BZD_BUNDLE="$0"
export BUILD_WORKSPACE_DIRECTORY="$(pwd)"
export RUNFILES_DIR="$temp"
cd "$temp/{cwd}"
"{executable}" {args} "$@" # No exec to activate trap EXIT
exit 0

#__END_OF_SCRIPT__#
""".format(executable=str(self.executable), cwd=str(self.cwd), stamp=stamp, args=" ".join(args)).encode("utf-8"))
				fileOut.write(fd.read())

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
	parser.add_argument("--manifest", help="The path of the manifest to be used.", required=True)
	parser.add_argument("--executable", help="The path of the executable within the archive.", required=True)
	parser.add_argument("rest", nargs="*")

	args = parser.parse_args()

	bundler = Bundler(
	    pathlib.Path(args.manifest),
	    pathlib.Path(args.executable),
	    pathlib.Path(args.cwd),
	)
	bundler.process(output=pathlib.Path(args.output), args=args.rest)

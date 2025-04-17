import argparse
import pathlib
import hashlib
import typing
import stat


def tarCompressionArgument(compression: str) -> str:
	compressionToArg = {
	    "gz": "-z",
	    "xz": "-f",
	    "bz2": "-x",
	}
	assert compression in compressionToArg, f"Unsupported compression algorithm '{compression}'."
	return compressionToArg[compression]


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Archive to script.")
	parser.add_argument(
	    "--output",
	    default=pathlib.Path("/tmp/output"),
	    type=pathlib.Path,
	    help="The output path for the self extracting bundle.",
	)
	parser.add_argument(
	    "--compression",
	    choices=["gz", "bz2", "xz"],
	    help="The type of compression for the tarball.",
	)
	parser.add_argument(
	    "--entry-point",
	    type=pathlib.Path,
	    help="The entry point script to run.",
	)
	parser.add_argument(
	    "archive",
	    type=pathlib.Path,
	    help="The path of the archive to add into the self extractable binary.",
	)
	parser.add_argument("args", nargs="*", help="The rest of the arguments to be passed to the entry point.")

	args = parser.parse_args()

	with args.archive.open(mode="rb") as fd:
		# Compute the checksum, this ensure reproductibility over builds.
		stamp = hashlib.md5(fd.read()).hexdigest()
		fd.seek(0)

		# Create the self extracting output file.
		with args.output.open(mode="wb") as fileOut:
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

	sed '0,/^#__END_OF_SCRIPT__#$/d' "$0" | tar -C "$temp" -x {compression}
	touch "$temp/{stamp}"
fi

export BZD_BUNDLE="$0"
export BUILD_WORKSPACE_DIRECTORY="${{BUILD_WORKSPACE_DIRECTORY:-$(pwd)}}"
cd "$temp"
"$temp/{entryPoint}" {args} "$@" # No exec to activate trap EXIT
exit 0

#__END_OF_SCRIPT__#
""".format(entryPoint=str(args.entry_point),
			stamp=stamp,
			compression=tarCompressionArgument(args.compression) if args.compression else "",
			args=" ".join(args.args)).encode("utf-8"))
			fileOut.write(fd.read())

	# Set permission to executable.
	args.output.chmod(args.output.stat().st_mode | stat.S_IEXEC)

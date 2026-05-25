import argparse
import pathlib
import shutil

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bundle a bazel executable into a single file.")
	parser.add_argument(
		"--output",
		default=pathlib.Path("/tmp/output"),
		type=pathlib.Path,
		help="The output path where the bundle should be copied.",
	)
	parser.add_argument(
		"--force",
		action="store_true",
		help="Force creating the output, will overwrite any existing file.",
	)
	parser.add_argument(
		"--bundle",
		type=pathlib.Path,
		help="The path of the original bundle.",
	)
	args = parser.parse_args()

	if args.output.exists() or args.output.is_symlink():
		args.output.unlink()

	shutil.copy2(args.bundle, args.output)
	print(f"Bundle available at {args.output}")

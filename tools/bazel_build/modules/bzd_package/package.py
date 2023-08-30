import argparse
import json
import pathlib
import tarfile

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Create a package from a manifest.")
	parser.add_argument(
	    "--output",
	    type=pathlib.Path,
	    default=pathlib.Path("out.tar"),
	    help="The path of the generated package.",
	)
	parser.add_argument(
	    "manifest",
	    type=pathlib.Path,
	    help="The manifest path.",
	)

	args = parser.parse_args()

	data = json.loads(args.manifest.read_text())
	with tarfile.open(args.output, mode="w", dereference=True) as tar:
		for targeted, actual in data.items():
			tar.add(actual, arcname=targeted)

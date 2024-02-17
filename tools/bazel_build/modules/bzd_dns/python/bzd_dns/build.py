import argparse
import pathlib
import typing
import json

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="DNS provider builder.")
	parser.add_argument("--output", default=None, type=pathlib.Path, help="Output directory.")
	parser.add_argument("--config", default=None, type=pathlib.Path, help="Configuration used for the provider.")
	parser.add_argument(
	    "--input",
	    nargs=2,
	    dest="inputs",
	    metavar=("domain", "path"),
	    action="append",
	    help="Input pair of zone files and their domain.",
	)

	args = parser.parse_args()

	print(args.inputs)
	sys.exit(1)

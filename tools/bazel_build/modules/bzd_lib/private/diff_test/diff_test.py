import argparse
import pathlib
import sys

from private.diff_test.compare_binary import compare as compareBinary
from private.diff_test.compare_json import compare as compareJson
from private.diff_test.compare_text import compare as compareText

compareFunctions = {"binary": compareBinary, "json": compareJson, "text": compareText}

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Test the equality of 2 files.")
	parser.add_argument(
	    "--format",
	    default="binary",
	    choices=["binary", "json", "text"],
	    help="The format of the files to be checked for equality.",
	)
	parser.add_argument(
	    "file1",
	    type=pathlib.Path,
	    help="The first file to be tested.",
	)
	parser.add_argument(
	    "file2",
	    type=pathlib.Path,
	    help="The second file to be tested.",
	)
	args = parser.parse_args()

	assert args.format in compareFunctions, f"The format requested ({args.format}) is not supported."

	result = compareFunctions[args.format](args.file1, args.file2)
	sys.exit(0 if result else 1)

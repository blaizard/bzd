import argparse
import pathlib
import typing
import json

from bzd.utils.dict import updateDeep

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Merge multiple JSON files together.")
	parser.add_argument("-o", "--output", default=None, type=pathlib.Path, help="The output path of the JSON file.")
	parser.add_argument(
	    "inputs",
	    nargs="*",
	    type=pathlib.Path,
	    help="JSON input files to be merged.",
	)

	args = parser.parse_args()

	output: typing.Dict[str, typing.Any] = {}
	for f in args.inputs:
		data = json.loads(f.read_text())
		updateDeep(output, data)

	outputJson = json.dumps(output, indent=4)

	if args.output:
		args.output.write_text(outputJson)
	else:
		print(outputJson)

import argparse
import pathlib
import typing
import json

from bzd_dns.zones import Zones

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="DNS zones aggregator and sanitizer.")
	parser.add_argument("--output", default=None, type=pathlib.Path, help="The output path of the JSON file.")
	parser.add_argument("--config", default=None, type=pathlib.Path, help="Configuration to be used for substitution.")
	parser.add_argument(
	    "inputs",
	    nargs="*",
	    type=pathlib.Path,
	    help="JSON input files to be merged.",
	)

	args = parser.parse_args()

	# Read substitutions.

	substitutions = {}
	if args.config:
		substitutions = json.loads(args.config.read_text())

	# Merge all the inputs together.
	zones = Zones()
	for f in args.inputs:
		dataStr = f.read_text()
		for k, v in substitutions.items():
			dataStr = dataStr.replace(f"${k}", v)
		data = json.loads(dataStr)
		zones.add(data)

	# Convert zones to json.
	data = zones.toJson()
	dataStr = json.dumps(data, sort_keys=True, indent=4)
	if args.output:
		args.output.write_text(dataStr)
	else:
		print(dataStr)

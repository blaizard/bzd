#!/usr/bin/python

import argparse
import json

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Information generator rules.")
	parser.add_argument("--input",
	                    nargs=2,
	                    action="append",
	                    help="Pair of application name and an information package json file, can add multiple.")
	parser.add_argument("--config", type=str, help="Build configuration.")
	parser.add_argument("output", default="package.manifest", help="Path of the generated file.")

	args = parser.parse_args()

	# Merge all entries together
	output = {"config": args.config, "fragments": {}}
	for item in args.input:
		with open(item[1]) as f:
			fragment = json.load(f)
		if item[0]:
			if item[0] not in output["fragments"]:
				output["fragments"][item[0]] = {}
			output["fragments"][item[0]].update(fragment)
		else:
			output.update(fragment)

	with open(args.output, "w") as f:
		f.write(json.dumps(output, indent=4, sort_keys=True))

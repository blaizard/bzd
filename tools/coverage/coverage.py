#!/usr/bin/python3

import argparse
import os
import re
import json
import subprocess

if __name__ == "__main__":

	parser = argparse.ArgumentParser()
	parser.add_argument("--output",
		help="Output path",
		default="bazel-out/coverage")
	parser.add_argument("--workspace",
		default=os.environ["BUILD_WORKSPACE_DIRECTORY"],
		help="Output path")
	parser.add_argument("--genhtml", help="Path for genhtml binary")
	parser.add_argument("--report",
		default="bazel-out/_coverage/_coverage_report.dat")
	args = parser.parse_args()

	# Build the report
	data = {
		"path": os.path.join(args.workspace, args.output, "index.html"),
		# Link relative to this file
		"link": "index.html",
		"coverage": 0,
		"lines": 0,
		"files": 0
	}

	# Count the number of files
	nbFiles = 0
	with open(os.path.join(args.workspace, args.report), "r") as f:
		for line in f:
			if line.startswith("SF:"):
				nbFiles += 1
	data["files"] = nbFiles

	# Generate the HTML output
	result = subprocess.run([
		os.path.abspath(args.genhtml), "--show-details", "--sort",
		"--function-coverage", "--prefix", args.workspace,
		"--output-directory", args.output, args.report
	],
		cwd=args.workspace,
		stdout=subprocess.PIPE,
		stderr=subprocess.STDOUT)
	output = result.stdout.decode(errors="ignore")
	assert result.returncode == 0, output

	m = re.search(r"(\d+)\s+of\s+(\d+)\s+lines", output)
	assert m, "Something went wrong in the report generation: {}".format(
		output)
	data["lines"] = int(m.group(2))
	data["coverage"] = float(int(m.group(1)) / int(m.group(2)))

	# Save the report
	with open(os.path.join(args.workspace, args.output, "report.json"),
		"w") as f:
		json.dump(data, f)

	print("Includes: {} files, {} lines".format(data["files"], data["lines"]))
	print("Coverage: {coverage:.2%}".format(  # type: ignore
		coverage=data["coverage"]))
	print("Report: {}".format(data["path"]))

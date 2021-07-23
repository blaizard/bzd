import argparse
import os
import re
import json
import subprocess
from pathlib import Path

from bzd.utils.run import localCommand

if __name__ == "__main__":

	parser = argparse.ArgumentParser()
	parser.add_argument("--output", help="Output path", type=Path, default=Path("bazel-out/coverage"))
	parser.add_argument("--workspace",
		type=Path,
		default=Path(os.environ["BUILD_WORKSPACE_DIRECTORY"]),
		help="Output path")
	parser.add_argument("--genhtml", type=Path, help="Path for genhtml binary")
	parser.add_argument("--report", type=Path, default=Path("bazel-out/_coverage/_coverage_report.dat"))
	args = parser.parse_args()

	# Build the report
	data = {
		"path": (args.workspace / args.output / "index.html").as_posix(),
		# Link relative to this file
		"link": "index.html",
		"coverage": 0,
		"lines": 0,
		"files": 0
	}

	# Cleanup path and count the number of files
	reportUpdate = Path("{}.update".format(args.report.as_posix()))
	with open(args.workspace / args.report, "r") as fin:
		with open(args.workspace / reportUpdate, "w") as fout:
			for line in fin:
				if line.startswith("SF:"):
					data["files"] += 1
					line = re.sub(r"^.*\.runfiles/__main__/", "SF:", line)
				fout.write(line)

	# Generate the HTML output
	result = localCommand([
		os.path.abspath(args.genhtml), "--show-details", "--sort", "--function-coverage", "--branch-coverage",
		"--demangle-cpp", "--prefix",
		args.workspace.as_posix(), "--output-directory",
		args.output.as_posix(),
		reportUpdate.as_posix()
	],
		cwd=args.workspace)

	m = re.search(r"(\d+)\s+of\s+(\d+)\s+lines", result.getStdout())
	assert m, "Something went wrong in the report generation: {}".format(result.output())
	data["lines"] = int(m.group(2))
	data["coverage"] = float(int(m.group(1)) / int(m.group(2)))

	# Save the report
	(args.workspace / args.output / "report.json").write_text(json.dumps(data))

	print("Includes: {} files, {} lines".format(data["files"], data["lines"]))
	print("Coverage: {coverage:.2%}".  # type: ignore
		format(coverage=data["coverage"]))
	print("Report: {}".format(data["path"]))

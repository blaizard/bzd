import argparse
import json
import sys
import pathlib
from typing import Any, Dict

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Information generator for NodeJs rules.")
	parser.add_argument(
	    "--package_json",
	    type=pathlib.Path,
	    default=pathlib.Path("packages.json"),
	    help="Path of the packages json file.",
	)
	parser.add_argument("output", type=pathlib.Path, default="package.manifest", help="Path of the generated file.")

	args = parser.parse_args()

	packagesJson = json.loads(args.package_json.read_text())
	packages = {name: {"requested": version} for name, version in packagesJson.get("dependencies", {}).items()}

	output = {"packages": packages}

	args.output.write_text(json.dumps(output))

	sys.exit(0)

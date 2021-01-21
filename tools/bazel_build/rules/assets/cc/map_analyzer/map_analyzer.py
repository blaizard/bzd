import argparse
import json
import sys
import pathlib

from tools.bazel_build.rules.assets.cc.map_analyzer.parser.clang import ParserClang
from tools.bazel_build.rules.assets.cc.map_analyzer.parser.gcc import ParserGcc

if __name__ == '__main__':

	parser = argparse.ArgumentParser(description="Map file analyzer.")
	parser.add_argument("map", type=pathlib.Path, help="Path of the map file to analyze.")
	parser.add_argument("output", help="Path of the json output.")

	args = parser.parse_args()

	for ParserType in [ParserGcc, ParserClang]:
		data = ParserType(args.map)
		if data.parse():
			break

	# Filter sections, most likely sections that will be stripped
	data.filter(".debug_*")
	data.filter(".comment")
	data.filter(".symtab")
	data.filter(".xt.lit*")
	data.filter(".xt.prop*")

	with open(args.output, "w+") as f:
		f.write(json.dumps({"size_groups": {"units": data.getByUnits(), "sections": data.getBySections()}}))

	sys.exit(0)

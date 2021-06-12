import argparse
from pathlib import Path

from tools.bdl.object import Object
from tools.bdl.lib import formatters, main, preprocess, build

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="BZD language parser and generator.")
	parser.add_argument("-o", "--output", default=None, type=Path, help="Output path of generated file.")
	parser.add_argument("--format", default="bdl", type=str, choices=formatters.keys(), help="Formatting type.")
	parser.add_argument("--stage", choices=["preprocess", "build"], help="Only perform a specific stage of the full process.")	
	parser.add_argument("input", type=Path, help="Input file to be passed to the parser.")

	config = parser.parse_args()

	if config.stage == "preprocess":

		bdl = preprocess(path=config.input)
		output = bdl.serialize()
	
	elif config.stage == "build":

		bdl = config.input.read_text()
		output = build(formatType=config.format, bdl=Object.fromSerialize(bdl))

	else:
		output = main(formatType=config.format, path=config.input)

	if config.output is None:
		print(output)
	else:
		config.output.write_text(output)
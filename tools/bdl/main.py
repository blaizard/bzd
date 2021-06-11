import argparse
from pathlib import Path

from tools.bdl.lib import formatters, main

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="BZD language parser and generator.")
	parser.add_argument("-o", "--output", default=None, type=Path, help="Output path of generated file.")
	parser.add_argument("--format", default="bdl", type=str, choices=formatters.keys(), help="Formatting type.")
	parser.add_argument("input", type=Path, help="Input file to be passed to the parser.")

	config = parser.parse_args()

	output = main(formatType=config.format, path=config.input)

	if config.output is None:
		print(output)
	else:
		config.output.write_text(output)
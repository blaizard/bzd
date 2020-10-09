import argparse
from pathlib import Path

from parser import Parser

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="BZD language parser and generator.")
	parser.add_argument("--output", default="./out", type=str, help="Output path of generated file.")
	parser.add_argument("inputs", nargs="*", type=Path, help="Input files to be passed to the parser.")

	config = parser.parse_args()

	for path in config.inputs:
		Parser(path).parse()

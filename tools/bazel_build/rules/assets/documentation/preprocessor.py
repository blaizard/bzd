import argparse
import pathlib
import typing
import os

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Preprocessor for Markdown file.")
	parser.add_argument("--output", type=pathlib.Path, help="Ouput path for the preprocessed markdown.")
	parser.add_argument("input", type=pathlib.Path, help="Input markdown.")

	args = parser.parse_args()

	args.output.write_text(args.input.read_text())

import argparse
import pathlib

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Preprocessor for Markdown file.")
	parser.add_argument("--output", type=pathlib.Path, help="Output path for the preprocessed markdown.")
	parser.add_argument("input", type=pathlib.Path, help="Input markdown.")

	args = parser.parse_args()

	args.output.write_text(args.input.read_text())

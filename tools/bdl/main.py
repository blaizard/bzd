import argparse
from pathlib import Path

from tools.bdl.object import Object, ObjectContext
from tools.bdl.lib import formatters, main, preprocess, generate

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="BZD language parser and generator.")
	parser.add_argument("-o", "--output", default=None, type=Path, help="Output path of generated file.")
	parser.add_argument("--format", default="bdl", type=str, choices=formatters.keys(), help="Formatting type.")
	parser.add_argument("--use-path",
		action="append",
		type=Path,
		help="Path to be used as a root directory to ssearch for included files.")
	parser.add_argument("--root", type=Path, default=Path(), help="Root directory to be used for inclusions.")
	parser.add_argument("--preprocess-format",
		default=None,
		type=str,
		help=
		"This is how is named a preprocessed file, used for auto-discovering preprocessed files instead of re-generating them."
						)
	parser.add_argument("--stage",
		choices=["preprocess", "generate"],
		help="Only perform a specific stage of the full process.")
	parser.add_argument("inputs", type=Path, nargs="+", help="Input file to be passed to the parser.")

	config = parser.parse_args()

	objectContext = ObjectContext(root=config.root, usePath=config.use_path, preprocessFormat=config.preprocess_format)

	for inputPath in config.inputs:

		if config.stage == "preprocess":

			preprocess(path=inputPath, objectContext=objectContext)

		else:
			if config.stage == "generate":

				bdl = objectContext.loadPreprocess(path=inputPath)
				output = generate(formatType=config.format, bdl=bdl)

			else:
				output = main(formatType=config.format, path=inputPath, objectContext=objectContext)

			if config.output is None:
				print(output)
			else:
				config.output.write_text(output, encoding="ascii")

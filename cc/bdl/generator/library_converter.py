import argparse
import pathlib

from bdl.object import ObjectContext
from cc.bdl.generator.impl.visitor import formatCc

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bdl library generator for the Rust language.")
	parser.add_argument(
		"--bdl",
		action="append",
		nargs=2,
		type=pathlib.Path,
		metavar=("BDL", "OUTPUT"),
		help="Preprocessed Bdl files and their associated output files.",
	)
	parser.add_argument("--include", action="append", default=[], type=pathlib.Path, help="Include files.")

	args = parser.parse_args()

	for inputPath, outputPath in args.bdl:
		bdl = ObjectContext().loadPreprocess(preprocess=inputPath)
		content = formatCc(
			bdl=bdl,
			data={
				"cc": {
					"includes": {"": [str(f) for f in args.include]},
				},
			},
		)
		outputPath.write_text(content)

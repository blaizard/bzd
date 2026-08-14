import argparse
import pathlib

from bdl.object import ObjectContext
from rust.bdl.generator.impl.visitor import formatRust

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bdl library generator for the Rust language.")
	parser.add_argument(
		"--bdl",
		action="append",
		type=pathlib.Path,
		help="Preprocessed Bdl files.",
	)
	parser.add_argument(
		"--include",
		action="append",
		type=str,
		help="Crate name of a bdl dependency to re-export.",
	)
	parser.add_argument(
		"--output",
		type=pathlib.Path,
		required=True,
		help="The path of the generated file.",
	)

	args = parser.parse_args()

	content = "#![no_std]\n\n"
	for include in args.include or []:
		content += f"pub use {include}::*;\n"
	content += "\n"
	for inputPath in args.bdl:
		bdl = ObjectContext().loadPreprocess(preprocess=inputPath)
		content += f"// Code generation for {inputPath}\n\n"
		content += formatRust(bdl)
		content += "\n"
	args.output.write_text(content)

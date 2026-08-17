import argparse
import pathlib

from bdl.object import ObjectContext
from rust.bdl.generator.impl.visitor import formatRust


def generateContent(
	bdlFiles: list[pathlib.Path],
	imports: list[str],
	includes: list[str],
) -> str:
	content = "#![no_std]\n\n"
	if imports:
		content += "#![allow(unused_imports)]\n\n"
	for import_ in imports:
		content += f"use {import_}::*;\n"
	for include in includes:
		content += f"pub use {include}::*;\n"
	if imports or includes:
		content += "\n"
	for inputPath in bdlFiles:
		bdl = ObjectContext().loadPreprocess(preprocess=inputPath)
		content += f"// Code generation for {inputPath}\n\n"
		content += formatRust(bdl)
		content += "\n"
	return content


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bdl library generator for the Rust language.")
	parser.add_argument(
		"--bdl",
		action="append",
		type=pathlib.Path,
		help="Preprocessed Bdl files.",
	)
	parser.add_argument(
		"--import",
		dest="imports",
		action="append",
		type=str,
		help="Crate name of a bdl dependency to import privately.",
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

	content = generateContent(
		bdlFiles=args.bdl or [],
		imports=args.imports or [],
		includes=args.include or [],
	)
	args.output.write_text(content)

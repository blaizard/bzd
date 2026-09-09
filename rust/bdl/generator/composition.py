import argparse
import pathlib

from bdl.object import ObjectContext
from bdl.visitors.composition.visitor import Composition
from rust.bdl.generator.impl.visitor import compositionRust

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bdl composition generator for the Rust language.")
	parser.add_argument(
		"--output",
		type=pathlib.Path,
		help="The output path.",
	)
	parser.add_argument(
		"--target",
		required=True,
		help="The target to process.",
	)
	parser.add_argument(
		"bdls",
		nargs="+",
		type=pathlib.Path,
		help="Preprocessed Bdl files.",
	)

	args = parser.parse_args()

	composition = Composition()
	for bdl in args.bdls:
		composition.visit(ObjectContext().loadPreprocess(preprocess=bdl))
	composition.process()

	content = compositionRust(
		composition=composition.view(args.target),
	)
	args.output.write_text(content)

import argparse
import pathlib

from bdl.object import ObjectContext
from bdl.visitors.composition.visitor import Composition
from bdl.generators.json.visitor import compositionJson

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bdl composition generator to a JSON file.")
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

	content = compositionJson(
		composition=composition.view(args.target),
	)
	args.output.write_text(content)

import argparse
import sys
from pathlib import Path

import bzd.parser.error
from tools.bdl.object import Object, ObjectContext
from tools.bdl.lib import formatters, main, preprocess, generate, compose

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="BZD language parser and generator.")
	parser.add_argument("-o", "--output", default=None, type=Path, help="Output path of generated file.")
	parser.add_argument("--format", default="bdl", type=str, choices=formatters.keys(), help="Formatting type.")
	parser.add_argument("--no-color", action="store_true", help="Don't use colors.")
	parser.add_argument("--namespace",
		default=None,
		type=str,
		help="Namespace to be injected in the preprocessed files.")
	parser.add_argument("--preprocess-format",
		default=None,
		type=str,
		help=
		"This is how is named a preprocessed file, used for auto-discovering preprocessed files instead of re-generating them."
						)
	parser.add_argument("--stage",
		choices=["preprocess", "generate", "compose"],
		help="Only perform a specific stage of the full process.")
	parser.add_argument("--data",
		type=Path,
		default=None,
		help="Data file to be added to the generation or composition phase.")
	parser.add_argument("--target",
		dest="targets",
		action="append",
		default=[],
		help="Generate the composition for the given targets.")
	parser.add_argument("inputs", type=str, nargs="+", help="Input file to be passed to the parser.")

	config = parser.parse_args()

	objectContext = ObjectContext(preprocessFormat=config.preprocess_format, resolve=True)

	# Set colors if running on a terminal
	bzd.parser.error.useColors = not config.no_color

	if config.stage == "compose":

		bdls = [(objectContext.loadPreprocess(source=source)) for source in config.inputs]
		compose(formatType=config.format,
			bdls=bdls,
			output=config.output,
			targets=set(config.targets),
			data=config.data)

	else:

		for source in config.inputs:

			if config.stage == "preprocess":

				preprocess(source=source, namespace=config.namespace, objectContext=objectContext)

			else:
				if config.stage == "generate":

					bdl = objectContext.loadPreprocess(source=source)
					output = generate(formatType=config.format, bdl=bdl, data=config.data)

				else:
					output = main(formatType=config.format,
						source=source,
						objectContext=objectContext,
						data=config.data)

				if config.output is None:
					print(output)
				else:
					config.output.write_text(output, encoding="ascii")

import argparse
from pathlib import Path

import bzd.parser.error
from bdl.object import ObjectContext
from bdl.lib import formatters, preprocess, compose

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="BZD language parser and generator.")
	parser.add_argument("-o", "--output", default=None, type=Path, help="Output path of generated file.")
	parser.add_argument(
		"--format",
		default="bdl",
		type=str,
		choices=formatters.keys(),
		help="Formatting type.",
	)
	parser.add_argument("--no-color", action="store_true", help="Don't use colors.")
	parser.add_argument(
		"--namespace",
		default=None,
		type=str,
		help="Namespace to be injected in the preprocessed files.",
	)
	parser.add_argument(
		"--search-path",
		dest="searchPaths",
		action="append",
		default=[],
		type=str,
		help="Search directory for preprocessed `.bdl.o` files from upstream rules and raw preset files.",
	)
	parser.add_argument(
		"--stage",
		choices=["preprocess", "compose"],
		required=True,
		help="Perform a specific stage of the full process.",
	)
	parser.add_argument(
		"--data",
		type=Path,
		default=None,
		help="Data file to be added to the generation or composition phase.",
	)
	parser.add_argument(
		"--target",
		dest="targets",
		action="append",
		default=[],
		help="Generate the composition for the given targets.",
	)
	parser.add_argument("inputs", type=str, nargs="+", help="Input file to be passed to the parser.")

	config = parser.parse_args()

	objectContext = ObjectContext(
		searchPaths=config.searchPaths,
		resolve=True,
	)

	# Set colors if running on a terminal
	bzd.parser.error.useColors = not config.no_color

	if config.stage == "compose":
		bdls = []
		for source in config.inputs:
			maybePreprocess = objectContext.findPreprocess(source=source)
			assert maybePreprocess, f"Source file '{source}' is not preprocessed."
			bdls.append(objectContext.loadPreprocess(preprocess=maybePreprocess))
		compose(
			formatType=config.format,
			bdls=bdls,
			output=config.output,
			targets=set(config.targets),
			data=config.data,
		)
	elif config.stage == "preprocess":
		for source in config.inputs:
			preprocess(
				source=source,
				namespace=config.namespace.split(".") if config.namespace else None,
				objectContext=objectContext,
			)

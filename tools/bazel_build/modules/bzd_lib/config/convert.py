import argparse
import pathlib
import json
import yaml

from config.reader import internalToDictionary

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Substitute template values.")
	parser.add_argument("--output", type=pathlib.Path, help="The output path for the generated file.")
	parser.add_argument(
		"--internal",
		type=pathlib.Path,
		help="The internal values in json format.",
		required=True,
	)
	parser.add_argument(
		"--format",
		choices=["yaml", "json"],
		type=str,
		help="The output format.",
		required=True,
	)

	args = parser.parse_args()
	internal = json.loads(args.internal.read_text())

	if args.format == "yaml":
		data, _ = internalToDictionary(internal)
		content = yaml.dump(data)  # type: ignore

	elif args.format == "json":
		data, _ = internalToDictionary(internal)
		content = json.dumps(data, indent=4)

	else:
		raise Exception(f"Unsupported format: {args.format}.")

	if args.output:
		args.output.write_text(content)
	else:
		print(content)

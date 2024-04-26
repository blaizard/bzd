import argparse
import pathlib
import typing
import json
import re

from bzd.template.template import Template
from config.reader import processKeyValues


class Helpers:

	@staticmethod
	def toJson(data: typing.Any) -> typing.Any:
		return json.dumps(data)

	@staticmethod
	def toSubsetChar(name: str) -> str:
		return re.sub(r"[^a-zA-Z0-9_]+", "_", name)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Substitute template values.")
	parser.add_argument("--output", type=pathlib.Path, help="The output path for the generated file.")
	parser.add_argument("--json", type=pathlib.Path, help="The input values in json format.")
	parser.add_argument(
	    "template",
	    type=pathlib.Path,
	    help="The template to substitute.",
	)

	args = parser.parse_args()

	if args.json:
		data = json.loads(args.json.read_text())
	else:
		raise Exception("Missing input data")

	dataProcessed = processKeyValues(data)
	template = Template.fromPath(args.template)
	content = template.render({"data": dataProcessed}, Helpers)

	if args.output:
		args.output.write_text(content)
	else:
		print(content)

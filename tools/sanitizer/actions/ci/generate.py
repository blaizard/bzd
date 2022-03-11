import argparse
import pathlib
import json

from bzd.template.template import Template

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Script to generate pipeline stages")
	parser.add_argument("--output", type=pathlib.Path, default="output.txt", help="Output path.")
	parser.add_argument("--filter", type=str, dest="filters", action="append", help="List of filters to be used.")
	parser.add_argument("stages", type=pathlib.Path, help="Stages description file.")
	parser.add_argument("template", type=pathlib.Path, help="Template file to be used.")

	args = parser.parse_args()

	data = json.loads(args.stages.read_text())
	template = Template.fromPath(args.template)

	# TODO fix condition evaluation
	output = template.render(data)

	print(output)

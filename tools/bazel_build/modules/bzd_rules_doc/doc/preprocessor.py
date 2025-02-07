import argparse
import pathlib
import re
import typing
import json
import subprocess


class Extensions:

	def __init__(self, files: typing.List[pathlib.Path]) -> None:
		self.data = []
		for file in files:
			extension = json.loads(file.read_text())
			self.data.append(extension)

	def execute(self, action: str) -> str:
		"""Run an action from the extensions."""

		# Look for all the matches.
		matches = [extension[action] for extension in self.data if action in extension]
		if not matches:
			raise Exception(f"There is no documentation extension for {action}.")

		# Ensure that all the executables point to the same file.
		executables = [m["executable"] for m in matches]
		if len(set(executables)) != 1:
			raise Exception(f"All executables must point to the same file: {str(matches)}.")

		# Combine all arguments.
		args = [arg for m in matches for arg in m["args"]]
		executable = executables[0]

		result = subprocess.run([executable] + args, stdout=subprocess.PIPE, universal_newlines=True, check=True)
		return result.stdout


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Preprocessor for Markdown file.")
	parser.add_argument("--output", type=pathlib.Path, help="Output path for the preprocessed markdown.")
	parser.add_argument("--extension", type=pathlib.Path, action="append", default=[], help="Extensions to be used.")
	parser.add_argument("input", type=pathlib.Path, help="Input markdown.")

	args = parser.parse_args()

	extensions = Extensions(args.extension)
	content = args.input.read_text()

	pattern = re.compile(r":::\s*(?P<action>[^\s(]+)")
	index = 0
	output = []
	for m in re.finditer(pattern, content):
		output.append(content[index:m.start()])
		output.append(extensions.execute(m["action"]))
		index = m.end()
	output.append(content[index:])

	args.output.write_text("".join(output))

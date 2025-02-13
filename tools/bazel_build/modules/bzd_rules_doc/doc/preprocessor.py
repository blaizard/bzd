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

	def replace(self, content: str) -> str:
		pattern = re.compile(r":::\s*(?P<action>[^\s(]+)")
		return pattern.sub(self.execute, content)

	def execute(self, m: re.Match) -> str:
		"""Run an action from the extensions."""

		action = m["action"]

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


class Builtin:

	def __init__(self, cwd: pathlib.Path) -> None:
		self.cwd = cwd

	def replace(self, content: str) -> str:
		pattern = re.compile(r"\$\(path\s+(?P<path>[^\)\s]+)\)")
		return pattern.sub(self.updatePath, content)

	def updatePath(self, m: re.Match) -> str:
		"""Replace path string: $(path <path>) into a relative path, relative to the current
		documentation being processed.
		"""

		path = pathlib.Path(m["path"].strip("/"))
		prefix = pathlib.Path("/".join([".."] * len(self.cwd.parents)))
		return str(prefix / path)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Preprocessor for Markdown file.")
	parser.add_argument("--output", type=pathlib.Path, help="Output path for the preprocessed markdown.")
	parser.add_argument("--extension", type=pathlib.Path, action="append", default=[], help="Extensions to be used.")
	parser.add_argument("input", type=pathlib.Path, help="Input markdown.")

	args = parser.parse_args()

	extensions = Extensions(args.extension)
	content = extensions.replace(args.input.read_text())

	# Update the path if any.
	builtin = Builtin(args.input.parent)
	content = builtin.replace(content)

	args.output.write_text(content)

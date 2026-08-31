import argparse
import ast
import json
import pathlib
import typing

from bzd_sanitizer.context import Context
from bzd_sanitizer.worker import worker
from bzd.utils.run import localBazelBinary


class SecretLocationFinder(ast.NodeVisitor):
	def __init__(self) -> None:
		self.nodes: typing.List[ast.Constant] = []

	def visit_Call(self, node: ast.Call) -> None:
		"""Find any bzd_secret with inline secret."""

		# Match `bzd_secret(...)` calls
		if isinstance(node.func, ast.Name) and node.func.id == "bzd_secret":
			for keyword in node.keywords:
				# Match keyword argument `content = "..."`
				if keyword.arg == "content" and isinstance(keyword.value, ast.Constant):
					value = keyword.value.value
					if isinstance(value, str):
						self.nodes.append(keyword.value)

		self.generic_visit(node)


def lineColToIndex(source: str, lineno: int, col: int) -> int:
	"""Converts 1-based lineno and 0-based col into a 0-based character index."""

	lines = source.splitlines(keepends=True)
	return sum(len(line) for line in lines[: lineno - 1]) + col


def workload(
	args: typing.Tuple[pathlib.Path, pathlib.Path, bool, pathlib.Path],
	stdout: typing.TextIO,
) -> bool:
	workspace, path, check, secret = args

	content = (workspace / path).read_text()
	tree = ast.parse(content, filename=path)
	finder = SecretLocationFinder()
	finder.visit(tree)

	isSuccess = True
	replacements = []
	for node in finder.nodes:
		value = node.value
		params = ["--payload", f"{path}:{node.lineno}", str(value)]
		params += ["check"] if check else ["encrypt"]

		result = localBazelBinary(
			secret.as_posix(),
			args=params,
			ignoreFailure=True,
			stdout=stdout,
			stderr=stdout,
		)
		isSuccess = isSuccess and result.isSuccess()
		if check:
			continue

		output = result.getStdout().strip()
		if output == value:
			continue

		# Update the secret within the file.
		assert node.lineno is not None
		assert node.col_offset is not None
		assert node.end_lineno is not None
		assert node.end_col_offset is not None
		indexStart = lineColToIndex(content, node.lineno, node.col_offset)
		indexEnd = lineColToIndex(content, node.end_lineno, node.end_col_offset)
		replacements.append((indexStart, indexEnd, json.dumps(output)))

	# Apply changes in REVERSE order to preserve text indices
	if replacements:
		replacements.sort(key=lambda x: x[0], reverse=True)
		for indexStart, indexEnd, serializedSecret in replacements:
			content = content[:indexStart] + serializedSecret + content[indexEnd:]
		(workspace / path).write_text(content)

	return isSuccess


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Update and check secrets in BUILD files.")
	parser.add_argument("--secret", type=pathlib.Path, help="The path of the secret binary.")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	context = Context.fromFile(args.context)

	worker(
		args.context,
		workload,
		args=(args.secret,),
		include=["**BUILD", "**BUILD.bazel"],
		excludeFile=".sanitizerignore",
	)

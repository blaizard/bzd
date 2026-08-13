import argparse
import pathlib
import json
import typing


def loadAndRepair(content: str) -> typing.Union[typing.Dict[str, typing.Any], typing.List[typing.Any]]:
	"""Load the longest valid json object from a string."""

	candidates = []
	pairs = {"}": "{", "]": "["}

	# Gather all potential candidates.
	indexStart = 0
	while indexStart < len(content):
		if content[indexStart] in "{[":
			stack = [content[indexStart]]
			for indexEnd in range(indexStart + 1, len(content)):
				c = content[indexEnd]
				if c in pairs:
					if stack.pop() != pairs[c]:
						break
				elif c in "{[":
					stack.append(c)
				else:
					continue
				if len(stack) == 0:
					candidates.append(content[indexStart : indexEnd + 1])
					indexStart = indexEnd
					break
		indexStart += 1

	# Sort by the largest first.
	candidates.sort(key=len, reverse=True)

	# Decode
	for candidate in candidates:
		try:
			result: typing.Union[typing.Dict[str, typing.Any], typing.List[typing.Any]] = json.loads(candidate)
			return result
		except json.JSONDecodeError:
			continue

	raise ValueError("No valid JSON object found in input.")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="JSON utility.")
	parser.add_argument("--file", type=pathlib.Path, help="The input file.")
	parser.add_argument(
		"--repair",
		action="store_true",
		help="Whether the input should be repaired (invalid json, or might have preamble, etc).",
	)
	parser.add_argument("--dump", action="store_true", help="Dump the json content on stdout.")
	args = parser.parse_args()

	content = None
	if args.file:
		content = args.file.read_text()
	assert content is not None, "Missing input."

	contentJson = loadAndRepair(content) if args.repair else json.loads(content)

	if args.dump:
		print(json.dumps(contentJson, indent=4))

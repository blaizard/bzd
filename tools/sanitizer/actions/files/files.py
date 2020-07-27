#!/usr/bin/python

import argparse
import json
import re
import sys
import bzd.utils.worker
import bzd.yaml
from pathlib import Path
from tools.sanitizer.utils.python.workspace import Files
from typing import TextIO, Any, Callable


def formatJson(path: str, stdout: TextIO) -> None:
	content = Path(path).read_text()
	try:
		parsedContent = json.loads(content)
	except json.JSONDecodeError as e:
		stdout.write("Cannot parse json file '{}:{}'\n".format(path, e.lineno))
		raise
	formattedContent = json.dumps(parsedContent, indent=4, sort_keys=True)
	Path(path).write_text(formattedContent, encoding="utf-8")


def formatYaml(path: str, stdout: TextIO) -> None:

	class Tag:

		def __init__(self, tagSuffix: str, value: str) -> None:
			self.tagSuffix = tagSuffix
			self.value = value

	def representer(dumper: Any, data: Tag) -> Any:
		return dumper.represent_scalar(data.tagSuffix, data.value)

	def defaultCtor(loader: Any, tagSuffix: str, node: Any) -> Tag:
		return Tag(tagSuffix, node.value)

	bzd.yaml.add_multi_constructor("", defaultCtor)
	bzd.yaml.add_representer(Tag, representer)

	content = Path(path).read_text()
	try:
		parsedContent = bzd.yaml.load(content, Loader=bzd.yaml.Loader)
	except Exception:
		stdout.write("Cannot parse yaml file '{}'\n".format(path))
		raise
	formattedContent = bzd.yaml.dump(parsedContent,
		default_flow_style=False,
		allow_unicode=True,
		indent=4,
		encoding=None)
	Path(path).write_text(formattedContent, encoding="utf-8")


def evaluateFiles(task: Callable[[str, TextIO], Any], workspace: Path, **kwargs: Any) -> bool:
	files = Files(workspace, **kwargs)
	worker = bzd.utils.worker.Worker(task)
	worker.start()
	for path in files.data():
		worker.add(path.as_posix())

	isSuccess = True
	for result in worker.data():
		if not result.isSuccess():
			isSuccess = False
			print(result.getOutput(), end="")

	worker.stop()

	return isSuccess


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Tool checking and formating json files.")
	parser.add_argument("workspace", type=Path, help="Workspace to be processed.")

	args = parser.parse_args()

	isSuccess = evaluateFiles(formatJson, args.workspace, include=["**/*.json"])
	isSuccess &= evaluateFiles(formatYaml, args.workspace, include=["**/*.yml"])

	sys.exit(0 if isSuccess else 1)

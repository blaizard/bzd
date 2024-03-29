import argparse
import json
import re
import sys
from pathlib import Path
from typing import TextIO, Any, Callable

import bzd.utils.worker
from bzd.parser.error import ExceptionParser
from tools.bdl.lib import main as bdlFormatter
from tools.sanitizer.utils.python.workspace import Files


def formatBdl(path: str, stdout: TextIO) -> None:
	content = Path(path).read_text()
	formattedContent = bdlFormatter(formatType="bdl", source=path)
	if formattedContent != content:
		Path(path).write_text(formattedContent, encoding="utf-8")


def evaluateFiles(task: Callable[[str, TextIO], Any], workspace: Path, **kwargs: Any) -> bool:
	files = Files(workspace, useGitignore=True, **kwargs)
	worker = bzd.utils.worker.Worker(task)
	try:
		worker.start()
		for path in files.data():
			worker.add(path.as_posix(), timeoutS=600)

		isSuccess = True
		for result in worker.data():
			if not result.isSuccess():
				isSuccess = False
				print(result.getOutput(), end="")
	finally:
		worker.stop()

	return isSuccess


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Tool checking and formatting json files.")
	parser.add_argument("workspace", type=Path, help="Workspace to be processed.")

	args = parser.parse_args()

	isSuccess = True
	isSuccess &= evaluateFiles(formatBdl, args.workspace, include=["**/*.bdl"])

	sys.exit(0 if isSuccess else 1)

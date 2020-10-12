import argparse
import os
import sys
import multiprocessing
from io import StringIO
from mypy.main import main
import bzd.utils.worker
from pathlib import Path
from tools.sanitizer.utils.python.workspace import Files
from typing import TextIO

configFile = os.path.join(os.path.dirname(__file__), "mypy.ini")


def mypyWorker(path: str, stdout: TextIO) -> None:
	
	main(script_path=None,
		stdout=stdout,
		stderr=stdout,
		args=[
		"--config-file", configFile, "--strict", "--no-incremental", "--follow-imports", "normal", "--pretty", path
		])


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wrapper for mypy")
	parser.add_argument("workspace", type=Path, help="Workspace to be processed.")

	args = parser.parse_args()

	files = Files(args.workspace, include=[
		"**/*.py",
	], exclude=["**python/bzd/yaml**", "**_test.py"])

	os.environ["MYPYPATH"] = "{workspace}:{workspace}/python".format(workspace = args.workspace)

	# Process the varous files
	worker = bzd.utils.worker.Worker(mypyWorker)
	worker.start()
	for path in files.data():
		worker.add(path.as_posix())

	isSuccess = True
	for result in worker.data():
		if not result.isSuccess():
			isSuccess = False
			print(result.getOutput(), end="")

	worker.stop()

	sys.exit(0 if isSuccess else 1)

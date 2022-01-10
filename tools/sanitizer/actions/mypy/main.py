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
includePaths = ["python"]


def mypyWorker(package: str, stdout: TextIO) -> None:

	# To support namespace, the package has to be passed
	main(script_path=None,
		stdout=stdout,
		stderr=stdout,
		clean_exit=True,
		args=["--config-file", configFile, "--strict", "--follow-imports", "normal", "--pretty", "-p", package])

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wrapper for mypy")
	parser.add_argument("workspace", type=Path, help="Workspace to be processed.")

	args = parser.parse_args()

	files = Files(args.workspace, include=[
		"**/*.py",
	], exclude=["**python/bzd/yaml**", "**_test.py"])

	# Set the include paths
	includeFullPaths = [args.workspace.as_posix()] + ["{}/{}".format(args.workspace, path) for path in includePaths]
	os.environ["MYPYPATH"] = ":".join(includeFullPaths)

	# Process the varous files
	worker = bzd.utils.worker.Worker(mypyWorker)
	worker.start()
	for path in files.data(relative=True):

		# Strip name from include paths (if available)
		# and generate package name
		pathStr = path.with_suffix("").as_posix()
		for strippedPath in includePaths:
			if pathStr.startswith(strippedPath):
				pathStr = pathStr[len(strippedPath) + 1:]
				break
		package = ".".join(pathStr.split("/"))

		worker.add(package)

	isSuccess = True
	for result in worker.data():
		if not result.isSuccess():
			isSuccess = False
			print(result.getOutput(), end="")

	worker.stop()

	sys.exit(0 if isSuccess else 1)

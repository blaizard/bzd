import argparse
import os
import sys
import bzd.utils.worker
from yapf.yapflib.yapf_api import FormatFile
from tools.sanitizer.utils.workspace import Files

configFile = os.path.join(os.path.dirname(__file__), "yapf.ini")
isSuccess = True


def yapfWorker(path, stdout):
	result = FormatFile(path,
		style_config=configFile,
		in_place=True,
		logger=stdout)
	assert result[1] == "utf-8", "Wrong encoding {}, must be utf-8".format(
		result[1])


def outputStream(result):
	if not result.isSuccess():
		global isSuccess
		isSuccess = False
		print(result.getOutput(), end="")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wrapper for mypy")
	parser.add_argument("workspace", help="Workspace to be processed.")

	args = parser.parse_args()

	files = Files(args.workspace,
		include=[
		"**/*.py",
		],
		exclude=[
		"**tools/bzd/generator/yaml**",
		])

	# Process the varous files
	worker = bzd.utils.worker.Worker(yapfWorker)
	worker.start()
	for path in files.data():
		worker.add(path)
	worker.stop(handler=outputStream)

	sys.exit(0 if isSuccess else 1)

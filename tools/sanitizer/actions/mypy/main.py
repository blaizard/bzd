import argparse
import os
import sys
import multiprocessing
from io import StringIO
from mypy.main import main
from tools.sanitizer.utils.workspace import Files

configFile = os.path.join(os.path.dirname(__file__), "mypy.ini")

def mypyWorker(path):
	output = StringIO()
	try:
		main(script_path = None, stdout = output, stderr = output, args = ["--config-file", configFile, "--no-incremental", "--follow-imports", "silent", "--pretty", path])
	except:
		print(output.getvalue(), end="")
		return 1
	return 0

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wrapper for mypy")
	parser.add_argument("workspace", help="Workspace to be processed.")

	args = parser.parse_args()

	files = Files(args.workspace, include = [
		"**/*.py",
	], exclude = [
		"**tools/bzd/generator/yaml**",
	])

	# Process the varous files
	p = multiprocessing.Pool(os.cpu_count())
	result = p.map(mypyWorker, files.data())

	sys.exit(0 if sum(result) == 0 else 1)

import argparse
import os
import sys
import multiprocessing
from io import StringIO
from mypy.main import main
import bzd.utils.worker
from tools.sanitizer.utils.workspace import Files

configFile = os.path.join(os.path.dirname(__file__), "mypy.ini")
isSuccess = True

def mypyWorker(path, stdout):
    main(script_path=None, stdout=stdout, stderr=stdout, args=[
        "--config-file", configFile, "--no-incremental", "--follow-imports", "silent", "--pretty", path])

def outputStream(result):
    if not result.isSuccess():
        global isSuccess
        isSuccess = False
        print(result.getOutput(), end = "")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Wrapper for mypy")
    parser.add_argument("workspace", help="Workspace to be processed.")

    args = parser.parse_args()

    files = Files(args.workspace, include=[
        "**/*.py",
    ], exclude=[
        "**tools/bzd/generator/yaml**",
    ])

    # Process the varous files
    worker = bzd.utils.worker.Worker(mypyWorker)
    worker.start()
    for path in files.data():
        worker.add(path)
    worker.stop(handler = outputStream)

    sys.exit(0 if isSuccess else 1)

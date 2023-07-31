import argparse
import pathlib
import os
import sys
import typing
import time

from bzd.utils.run import localCommand
from context import Context


def getFileList(workspace: pathlib.Path, path: pathlib.Path, all: bool) -> typing.List[str]:

	# Compute the list of files to sanitize.
	if all:
		result = localCommand(["git", "ls-files", path], cwd=workspace)
	else:
		result = localCommand(["git", "ls-files", "--other", "--modified", "--exclude-standard", path], cwd=workspace)
	fileList = list(filter(len, result.getStdout().split("\n")))

	# If there are no files, try a diff with the last commit.
	if len(fileList) == 0:
		result = localCommand(["git", "diff", "--name-only", "--diff-filter=ACMRU", "HEAD~1", path], cwd=workspace)
		fileList = list(filter(lambda x: (workspace / x).is_file(), result.getStdout().split("\n")))

	return fileList


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Workspace sanitizer.")
	parser.add_argument("-u", "--use", type=str, action="append", default=[], help="Actions to be used.")
	parser.add_argument("-a", "--all", action="store_true", help="Sanitize all files in the workspace.")
	parser.add_argument("-c", "--check", action="store_true", help="Only check if the files need to be sanitized.")
	parser.add_argument("-w",
	                    "--workspace",
	                    type=pathlib.Path,
	                    default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", "."),
	                    help="The path of the workspace.")
	parser.add_argument("path", type=pathlib.Path, default=".", nargs="?", help="The path to sanitize.")

	args = parser.parse_args()

	fileList = getFileList(workspace=args.workspace, path=args.path, all=args.all)
	if len(fileList) == 0:
		print("There is nothing to do.")
		sys.exit(0)

	contextFilePath = args.workspace / ".sanitizer~"
	context = Context(workspace=args.workspace, fileList=fileList)
	context.toFile(path=contextFilePath)

	if args.check:
		message = "Checking"
	else:
		message = "Sanitizing"

	success = True
	try:
		print(f"{message} {context.size()} file(s) in {args.workspace / args.path}:")
		for action in args.use:
			print(f"\t- {action}...", end="", flush=True)
			startTime = time.monotonic()

			status = "PASSED" if result.getReturnCode() == 0 else "FAILED"
			elapsedTime = time.monotonic() - startTime
			print(f" {status} ({elapsedTime:.1f}s)")

	finally:
		contextFilePath.unlink()

	sys.exit(0 if success else 1)

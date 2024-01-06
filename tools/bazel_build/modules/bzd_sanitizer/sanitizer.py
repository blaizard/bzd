import argparse
import pathlib
import os
import sys
import typing
import time

from bzd.utils.run import localCommand, localBazelTarget  # type: ignore
from bzd_sanitizer.context import Context


def getFileListFromResult(result, workspace: pathlib.Path) -> typing.List[str]:
	return list({f for f in result.getStdout().split("\n") if (workspace / f).is_file()})

def getFileListFromRecursiveGit(workspace: pathlib.Path, path: pathlib.Path, command: typing.List[str]):
	"""Execute a git command to get a list of files and searching within submodules as well."""

	result = localCommand(
	    command + [
	        str(path),
	    ],
	    cwd=workspace,
	)
	output = getFileListFromResult(result, workspace=workspace)

	result = localCommand(["git", "submodule", "--quiet", "foreach", " ".join(command) + " | sed \"s|^|$path/|\""],
	                      cwd=workspace)
	output += [f for f in getFileListFromResult(result, workspace=workspace) if (workspace / f).is_relative_to(workspace / path)]

	return output


def getFileList(workspace: pathlib.Path, path: pathlib.Path, all: bool) -> typing.List[str]:
	# Compute the list of files to sanitize.
	if all:
		fileList = getFileListFromRecursiveGit(
		    workspace=workspace,
		    path=path,
		    command=["git", "ls-files", "--cached", "--other", "--modified", "--exclude-standard"])
	else:
		fileList = getFileListFromRecursiveGit(
		    workspace=workspace, path=path, command=["git", "ls-files", "--other", "--modified", "--exclude-standard"])

	# If there are no files, try a diff with the last commit.
	if len(fileList) == 0:
		result = localCommand(
		    ["git", "diff", "--name-only", "--diff-filter=ACMRU", "HEAD~1",
		     str(path)],
		    cwd=workspace,
		)
		fileList = getFileListFromResult(result, workspace=workspace)

	return fileList


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Workspace sanitizer.")
	parser.add_argument("-u", "--use", type=str, action="append", default=[], help="Actions to be used.")
	parser.add_argument("-a", "--all", action="store_true", help="Sanitize all files in the workspace.")
	parser.add_argument(
	    "-c",
	    "--check",
	    action="store_true",
	    help="Only check if the files need to be sanitized.",
	)
	parser.add_argument(
	    "--color",
	    action="store_true",
	    default=sys.stdout.isatty(),
	    help="If set, the output will have colors.",
	)
	parser.add_argument(
	    "-w",
	    "--workspace",
	    type=pathlib.Path,
	    default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", "."),
	    help="The path of the workspace.",
	)
	parser.add_argument("path", type=pathlib.Path, default=".", nargs="?", help="The path to sanitize.")

	args = parser.parse_args()

	fileList = getFileList(workspace=args.workspace, path=args.path, all=args.all)
	if len(fileList) == 0:
		print("There is nothing to do.")
		sys.exit(0)

	contextFilePath = args.workspace / ".sanitizer~"
	context = Context(
	    workspace=args.workspace,
	    fileList=fileList,
	    check=args.check,
	    colors=args.color,
	)
	context.toFile(path=contextFilePath)

	success = True
	try:
		print(
		    f"{'Checking' if context.check else 'Sanitizing'} {context.size()} file(s) in {args.workspace / args.path}:"
		)
		for action in args.use:
			printActionContext = context.printAction(action)
			startTime = time.monotonic()

			result = localBazelTarget(
			    action,
			    [str(contextFilePath)],
			    ignoreFailure=True,
			    timeoutS=600,
			    cwd=args.workspace,
			)

			passed = result.getReturnCode() == 0
			elapsedTime = time.monotonic() - startTime
			printActionContext.printStatus(passed, elapsedTime)

			if not passed:
				context.printSection(title="ERROR", level=1)
				print(result.getOutput(), end="")
				context.printSection(title="COMMAND", level=1)
				print(f"{context.color('command')}bazel run {action} -- {contextFilePath}{context.color()}")
				success = False

	finally:
		if success:
			contextFilePath.unlink()

	sys.exit(0 if success else 1)

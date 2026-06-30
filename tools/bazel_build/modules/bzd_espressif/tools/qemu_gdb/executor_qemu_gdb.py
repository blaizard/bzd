import argparse
import pathlib
import typing
import sys
import os
import threading
import time

from bzd.utils.run import localCommand, Cancellation


def splitEndOfOptionsMarker(
	args: typing.List[str],
) -> typing.Tuple[typing.List[str], typing.List[str]]:
	"""Split the given argument list at the end-of-options marker if any."""
	try:
		index = args.index("--")
		return args[0:index], args[index + 1 :]
	except ValueError:
		return args, []


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="GDB executor script.")
	parser.add_argument("--qemu", type=pathlib.Path, required=True, help="The path of QEMU.")
	parser.add_argument("--gdb", type=pathlib.Path, required=True, help="The path of GDB.")
	parser.add_argument(
		"--binary",
		required=True,
		type=pathlib.Path,
		help="The binary containing the symbols used to decode the stack trace.",
	)
	parser.add_argument(
		"--workspace",
		type=pathlib.Path,
		default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", os.environ.get("BUILD_WORKING_DIRECTORY", ".")),
		help="The path of the workspace.",
	)
	scriptArgs, qemuArgs = splitEndOfOptionsMarker(sys.argv[1:])
	args = parser.parse_args(scriptArgs)

	qemuCancellation = Cancellation()

	def runQemu() -> None:
		localCommand(
			[str(args.qemu), "-s", "-S"] + qemuArgs, timeoutS=None, ignoreFailure=True, cancellation=qemuCancellation
		)

	thread = threading.Thread(target=runQemu)
	thread.start()

	print("Wait for QEMU to be ready...")
	time.sleep(1)

	try:
		localCommand(
			[
				str(args.gdb),
				str(args.binary),
				"-ex",
				"target remote :1234",
				"-ex",
				f"directory {args.workspace.absolute()}",
				"-ex",
				"break main",
				"-ex",
				"continue",
			],
			stdout=True,
			stderr=True,
			stdin=True,
			timeoutS=None,
		)
	finally:
		print("Terminating QEMU process, this might take a few seconds...")
		qemuCancellation.cancel()
		thread.join()

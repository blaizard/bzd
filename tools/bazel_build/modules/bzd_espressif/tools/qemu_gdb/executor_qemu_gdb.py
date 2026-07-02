import argparse
import pathlib
import typing
import sys
import os
import threading
import time
import pty
import socket

from bzd.utils.run import localCommand, localBazel, Cancellation


def splitEndOfOptionsMarker(
	args: typing.List[str],
) -> typing.Tuple[typing.List[str], typing.List[str]]:
	"""Split the given argument list at the end-of-options marker if any."""
	try:
		index = args.index("--")
		return args[0:index], args[index + 1 :]
	except ValueError:
		return args, []


def checkConnection(host: str, port: int, timeoutS: int) -> bool:
	"""Check that a connection exists."""

	timeStart = time.time()

	while time.time() - timeStart < timeoutS:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.settimeout(timeoutS)
		try:
			sock.connect((host, port))
		except Exception:
			continue
		else:
			sock.close()
			return True
	return False


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

	# Look for the external directory
	outputBase = pathlib.Path(localBazel(["info", "output_base"], cwd=args.workspace).getStdout().strip())

	qemuCancellation = Cancellation()

	def runQemu() -> None:
		localCommand(
			[str(args.qemu), "-s", "-S"] + qemuArgs, timeoutS=None, ignoreFailure=True, cancellation=qemuCancellation
		)

	thread = threading.Thread(target=runQemu)
	thread.start()

	try:
		print("Wait for QEMU to be ready (5 seconds)...")
		if not checkConnection(host="127.0.0.1", port=1234, timeoutS=5):
			print("QEMU did not start in time, terminating.")
			sys.exit(1)

		pty.spawn(
			[
				str(args.gdb),
				str(args.binary),
				"-ex",
				"target remote :1234",
				"-ex",
				f"directory '{args.workspace.absolute()}' '{outputBase.absolute()}'",
				"-ex",
				"break main",
				"-ex",
				"continue",
			],
		)
	finally:
		print("Terminating QEMU process.")
		qemuCancellation.cancel()
		thread.join()

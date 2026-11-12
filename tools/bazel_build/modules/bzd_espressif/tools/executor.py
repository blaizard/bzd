import argparse
import sys
import re
import shutil
import pathlib
import typing

from bzd.utils.run import Cancellation, localCommand, StreamCallback

EXIT_CODE_PATTERN = re.compile(r"<exit code (-?[0-9]+)>")


def tryDecodeBacktrace(binary: pathlib.Path, output: str) -> None:
	"""Identify if there is backtrace data and decode it if possible.

	Args:
	        binary: The binary to get the symbol from.
	        output: The output string.
	"""

	def findStackTrace(output: str) -> typing.Optional[typing.List[str]]:
		for line in output.split("\n"):
			if line.strip().startswith("Backtrace: "):
				return line.strip().split(" ")[1:]
		return None

	maybeStackTrace = findStackTrace(output)
	if maybeStackTrace is None:
		return
	maybeAddr2line = shutil.which("addr2line")
	if maybeAddr2line is None:
		print("Install 'addr2line' from binutils to decode the stack trace.")
		return

	print("Post processed stack trace:")
	for index, address in enumerate(maybeStackTrace):
		result = localCommand([maybeAddr2line, "-f", "-C", "-e", str(binary), address])
		symbol, source = result.getOutput().split("\n")[0:2]
		print(f"#{index} {address} in {symbol} {source}", flush=True)


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
	parser = argparse.ArgumentParser(description="QEMU executor script.")
	parser.add_argument("--emulator", type=pathlib.Path, required=True, help="The path of the emulator.")
	parser.add_argument(
		"--binary",
		type=pathlib.Path,
		help="The binary containing the symbols used to decode the stack trace.",
	)
	scriptArgs, emulatorArgs = splitEndOfOptionsMarker(sys.argv[1:])
	args = parser.parse_args(scriptArgs)

	# Note, wrapping QEMU within a python script allow easy killing of the process.
	# Otherwise we cannot simply use CTRL+C.
	# The emulator keeps running after the target exits, stop it once the exit code is reported.
	# LINE mode ensures the complete exit code is received before the process is terminated.
	cancellation = Cancellation()
	maybeExitCode: typing.Optional[int] = None

	def handleLine(line: str, stream: typing.TextIO) -> None:
		global maybeExitCode
		print(line, file=stream, flush=True)
		m = EXIT_CODE_PATTERN.search(line)
		if m is not None:
			maybeExitCode = int(m.group(1))
			cancellation.cancel()

	stdout = StreamCallback(callback=lambda line: handleLine(line, sys.stdout), mode=StreamCallback.Mode.LINE)
	stderr = StreamCallback(callback=lambda line: handleLine(line, sys.stderr), mode=StreamCallback.Mode.LINE)
	result = localCommand(
		[str(args.emulator)] + emulatorArgs,
		stdout=stdout,  # type: ignore
		stderr=stderr,  # type: ignore
		timeoutS=None,
		ignoreFailure=True,
		cancellation=cancellation,
	)

	# Try to decode the stack trace if any.
	if args.binary:
		tryDecodeBacktrace(args.binary, result.getOutput())

	if maybeExitCode is not None:
		sys.exit(maybeExitCode)
	sys.exit(42)

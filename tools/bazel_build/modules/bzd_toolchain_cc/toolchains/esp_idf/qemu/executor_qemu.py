import argparse
import sys
import re
import shutil
import pathlib
import typing

from bzd.utils.run import localCommand


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


def tryReadExitCode(output: str) -> typing.Optional[int]:
	"""Identify the exit code if any.
	
	Args:
		binary: The binary to get the symbol from.
		output: The output string.
	"""

	m = re.search(r"<exit code (-?[0-9])>", output)
	if m is None:
		return None
	return int(m.group(1))


def splitEndOfOptionsMarker(args: typing.List[str]) -> typing.Tuple[typing.List[str], typing.List[str]]:
	"""Split the given argument list at the end-of-options marker if any."""
	try:
		index = args.index("--")
		return args[0:index], args[index + 1:]
	except ValueError:
		return args, []


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="QEMU executor script.")
	parser.add_argument("--qemu", type=pathlib.Path, required=True, help="The path of QEMU.")
	parser.add_argument("--binary",
	                    type=pathlib.Path,
	                    help="The binary containing the symbols used to decode the stack trace.")
	scriptArgs, qemuArgs = splitEndOfOptionsMarker(sys.argv[1:])
	args = parser.parse_args(scriptArgs)

	# Note, wrapping QEMU within a python script allow easy killing of the process.
	# Otherwise we cannot simply use CTRL+C.
	result = localCommand([str(args.qemu)] + qemuArgs, stdout=True, stderr=True, timeoutS=None)

	# Try to decode the stack trace if any.
	if args.binary:
		tryDecodeBacktrace(args.binary, result.getOutput())

	# Decode the exit code if any.
	maybeExitCode = tryReadExitCode(result.getOutput())
	if maybeExitCode is not None:
		sys.exit(maybeExitCode)
	sys.exit(42)

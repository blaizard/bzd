import argparse
import pathlib
import typing
import sys


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
		type=pathlib.Path,
		help="The binary containing the symbols used to decode the stack trace.",
	)
	scriptArgs, qemuArgs = splitEndOfOptionsMarker(sys.argv[1:])
	args = parser.parse_args(scriptArgs)

	# Run build/qemu-system-xtensa -s -S {qemuArgs}
	# Then run xtensa-esp32-elf-gdb build/app-name.elf -ex "target remote :1234" -ex "monitor system_reset" -ex "tb app_main" -ex "c"

	print(qemuArgs)

import pathlib
import argparse
import os
import re
import typing
import threading
import time
import sys
import random

from bzd.utils.run import localDocker
from fragments.esp32.esptool.targets import targets


def createFlash(path: pathlib.Path, size: int, content: typing.Dict[int, pathlib.Path]) -> None:
	"""Assembles the file binary with its bootloader and partition to create the flash."""

	with open(path, "wb") as fout:
		# Create the full flash.
		fout.seek(size - 1)
		fout.write(b"\0")

		# Compose the flash with content.
		for offset, source in content.items():
			with open(source, "rb") as fin:
				fout.seek(offset)
				fout.write(fin.read())


def runGdb(name: str) -> None:
	# Wait until the container is available
	print(f"Waiting for container {name} to be up...")
	counter = 50
	while counter:
		if (localDocker(
		    ["exec", name, "/bin/bash", "-c", "exit", "0"],
		    ignoreFailure=True,
		    timeoutS=1,
		).getReturnCode() == 0):
			break
		time.sleep(0.1)
		counter -= 1
	assert counter, "Waiting for container timed-out."

	# Start gdb + gdbgui
	localDocker(
	    [
	        "exec",
	        "-t",
	        name,
	        "gdbgui",
	        "-r",
	        "--port=8080",
	        "-g",
	        "xtensa-esp32-elf-gdb",
	    ],
	    stdin=True,
	    stdout=True,
	    stderr=True,
	    ignoreFailure=True,
	    timeoutS=0,
	)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="ESP32 QEMU launcher script.")
	parser.add_argument("--target", choices=targets.keys(), default="esp32", help="Target.")
	parser.add_argument(
	    "--debug",
	    default=False,
	    action="store_true",
	    help="Use GDB to debug the target.",
	)
	parser.add_argument(
	    "--name",
	    default=f"xtensa_qemu_{os.getpid()}_{time.time_ns()}_{random.randrange(sys.maxsize)}",
	    help="Name of the container.",
	)
	parser.add_argument("elf", type=str, help="Binary in ELF format to be executed.")
	parser.add_argument("image", type=str, help="Binary image to be executed.")

	args = parser.parse_args()
	target = targets[args.target]

	# Create the flash.
	flashPath = pathlib.Path("flash.bin")
	createFlash(
	    flashPath,
	    typing.cast(int, target["memorySize"]),
	    {
	        offset: pathlib.Path(f.format(binary=args.image))
	        for offset, f in target["memoryMap"].items()
	    },
	)

	if args.debug:
		gdb = threading.Thread(target=runGdb, args=(args.name, ))
		gdb.start()

	# Make sure all the input files exist.
	gdbinit = pathlib.Path(f"{pathlib.Path(__file__).parent}/qemu/.gdbinit")
	assert gdbinit.is_file()

	print(flashPath.resolve().as_posix())
	os.system(f"ls -lla {flashPath.resolve().as_posix()}")
	os.system(f"cp {flashPath.resolve().as_posix()} dummy.bin")
	os.system(f"ls -lla")
	print(pathlib.Path.cwd() / "dummy.bin")

	cmds = [
	    "run",
	    "-t",
	    "--rm",
	    "--name",
	    args.name,
	    #f"--volume={flashPath.resolve().as_posix()}:/bzd/flash.bin:rw",
	    "--mount", f"source={pathlib.Path.cwd() / 'dummy.bin'},target=/bzd/flash.bin,type=bind",
	    f"--volume={gdbinit.resolve().as_posix()}:/root/.gdbinit:ro",
	    f"--volume={pathlib.Path(args.elf).resolve().as_posix()}:/bzd/binary.bin:ro",
	]

	if args.debug:
		assert "BUILD_WORKSPACE_DIRECTORY" in os.environ
		cmds += [
		    "-p",
		    "8080",
		    "--volume={}:/code:ro".format(os.environ["BUILD_WORKSPACE_DIRECTORY"]),
		]

	cmds += [
	    "blaizard/xtensa_qemu:latest",
	    "qemu-system-xtensa",
	    "-no-reboot",
	    "-nographic",
	    "-machine",
	    "esp32",
	    "-m",
	    "4",
	    "-drive",
	    "file=/bzd/flash.bin,if=mtd,format=raw",
	]

	if args.debug:
		cmds += ["-gdb", "tcp::1234", "-S"]

	try:
		result = localDocker(cmds, stdin=False, stdout=True, stderr=True, ignoreFailure=True, timeoutS=0)

		if args.debug:
			gdb.join()

		m = re.search(r"<simulation exit code (-?\d+)>\s*$", result.getOutput())
		if m:
			sys.exit(int(m.group(1)))
		sys.exit(42)

	finally:
		# Cleanup everything
		localDocker(["stop", "--time=5", args.name], ignoreFailure=True, timeoutS=10)

import pathlib
import argparse
import os
import typing
import threading
import time
import sys

from python.bzd.utils.run import localDockerComposeRun, localDocker
from toolchains.cc.fragments.esptool.targets import targets


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


def runGdb() -> None:
	input("Press ENTER to connect gdb...\n")
	localDocker(
		[
		"exec", "-it", "xtensa_qemu", "gdbgui", "-r", "--port=8080", "-g",
		"xtensa-esp32-elf-gdb"
		],
		stdin=True,
		stdout=True,
		stderr=True,
		timeoutS=0)

	localDocker(["stop", "--time=5", "xtensa_qemu"], timeoutS=10)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="ESP32 QEMU launcher script.")
	parser.add_argument("--target", choices=targets.keys(), default="esp32", help="Target.")
	parser.add_argument('--gdb', default=False, action="store_true", help="Use GDB to debug the target.")
	parser.add_argument("elf", type=str, help="Binary in ELF format to be executed.")
	parser.add_argument("image", type=str, help="Binary image to be executed.")

	args = parser.parse_args()

	target = targets[args.target]

	# Create the flash.
	flashPath = pathlib.Path("flash.bin")
	createFlash(flashPath, typing.cast(int, target["memorySize"]),
		{offset: pathlib.Path(f.format(binary=args.image))
		for offset, f in target["memoryMap"].items()})  # type: ignore

	if args.gdb:
		gdb = threading.Thread(target=runGdb)
		gdb.start()

	cmds = [
		"run",
		"-t",
		"-p",
		"8080",
		"--volume={}:/bzd/flash.bin:rw".format(flashPath.resolve().as_posix()),
		"--volume={}:/root/.gdbinit:ro".format(
		pathlib.Path("toolchains/cc/fragments/esptool/qemu/.gdbinit").resolve().as_posix()),
		"--volume={}:/bzd/binary.bin:ro".format(pathlib.Path(args.elf).resolve().as_posix()),
		"--volume={}:/code:ro".format(os.environ["BUILD_WORKSPACE_DIRECTORY"]),
		"--name=xtensa_qemu",
		"--rm",
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
		"-nic",
		"user,model=open_eth,hostfwd=tcp::80-:80"
	]

	if args.gdb:
		cmds += ["-gdb", "tcp::1234", "-S"]

	localDocker(cmds, stdin=False, stdout=True, stderr=True, timeoutS=0)

	if args.gdb:
		gdb.join()

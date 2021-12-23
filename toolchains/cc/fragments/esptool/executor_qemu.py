import pathlib
import argparse
import os
import typing

from python.bzd.utils.run import localDockerComposeBinary
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


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="ESP32 QEMU launcher script.")
	parser.add_argument("--target", choices=targets.keys(), default="esp32", help="Target.")
	parser.add_argument("binary", type=str, help="Binary to be executed.")

	args = parser.parse_args()

	target = targets[args.target]

	# Create the flash.
	flashPath = pathlib.Path("flash.bin")
	createFlash(
		flashPath, target["memorySize"], {offset: pathlib.Path(f.format(binary = args.binary)) for offset, f in target["memoryMap"].items()})

	# Update the docker-compose template file and write it to the file.
	dockerCompose = pathlib.Path("toolchains/cc/fragments/esptool/qemu/docker_compose.yml.template").read_text().format(
		flash=flashPath.resolve().as_posix())
	dockerComposePath = pathlib.Path("docker-compose.yml")
	dockerComposePath.write_text(dockerCompose)

	# Execute docker compoase
	localDockerComposeBinary(dockerComposePath, stdout=True, stderr=True, timeoutS=0)

import pathlib
import typing
import argparse
import sys
import time
import threading

from apps.bootloader.config import endpoint, uid
from apps.artifacts.plugins.fs.release.python.release import Release, Update
from apps.bootloader.manifest import Manifest
from apps.bootloader.singleton import Singleton
from bzd.utils.logging import Logger
from bzd.utils.run import localCommand


class Bootloader:

	def __init__(self, root: pathlib.Path) -> None:
		self.root = root
		self.bin = pathlib.Path("bin")
		self.manifest = Manifest(root / "manifest.json")
		self.release = Release(endpoint=endpoint, uid=uid)
		self.mutex = threading.Lock()

	def getBinary(self) -> pathlib.Path:
		"""Get the binary to run.

		This function checks for the validity of the binary and raise an exception on error.
		"""

		maybeBinary = self.manifest.maybeBinary

		# If there is no binary, download one.
		if maybeBinary is None:
			maybeBinary = self.update()
			if maybeBinary is None:
				raise Exception("There is no binary, nor update available.")

		assert maybeBinary is not None, "Binary must exist."
		binary = self.root / maybeBinary
		if not binary.is_file():
			with self.mutex:
				self.manifest.setBinary(None)
			raise Exception(f"The binary '{binary}' must point to an existing file.")

		return binary

	def rollback(self) -> bool:
		"""Rollback to the previous stable version."""

		with self.mutex:
			self.manifest.setUnstable()
			maybeStable = self.manifest.maybeStable
			self.manifest.setBinary(maybeStable)
			return True if maybeStable else False

	def update(self, update: Update) -> typing.Optional[pathlib.Path]:
		"""Try to update the binary and return the relative binary path if any."""

		with self.mutex:

			# Perform the update
			maybeLastUpdate = self.manifest.maybeLastUpdate
			maybeUpdate = self.release.fetch(after=maybeLastUpdate.name if maybeLastUpdate else None)
			if not maybeUpdate:
				return None

			# Set the binary.
			path = self.bin / maybeUpdate.name
			maybeUpdate.toFile(self.root / path)
			self.manifest.setBinary(path)
			self.manifest.setUpdate(path, int(time.time()))

			# Cleanup the bin folder, keep only the stable binary and this one.
			maybeStable = self.manifest.maybeStable
			keep = {maybeStable.name, path.name} if maybeStable else {path.name}
			for f in (self.root / self.bin).iterdir():
				if f.name not in keep:
					try:
						f.unlink()
					except Exception as e:
						Logger.error(f"Cannot delete file {f}")

			return path

	def run(self, args: typing.List[str]) -> None:
		"""Run the binary."""

		def markBinaryAsStable():
			with self.mutex:
				self.manifest.setStable()

		binary = self.getBinary()
		timer = threading.Timer(3600, markBinaryAsStable)
		try:
			timer.start()
			localCommand(cmds=[binary] + args, timeoutS=None, stdout=True, stderr=True)
		finally:
			timer.stop()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bootloader.")
	parser.add_argument("path", type=pathlib.Path, help="The directory where to store application images.")
	parser.add_argument("uid", type=str, help="The unique identifier of the application.")

	args = parser.parse_args()

	# Make sure only one instance of the bootloader is running at a time.
	singleton = Singleton(args.path / "singleton.lock")
	if not singleton.lock():
		sys.exit(0)

	bootloader = Bootloader(args.path)

	lastFailure = 0
	while True:
		try:
			bootloader.run()
			sys.exit(0)
		except KeyboardInterrupt:
			Logger.info("<<< Keyboard interrupt >>>")
			sys.exit(130)
		except Exception as e:
			Logger.error(e)
			if bootloader.rollback():
				Logger.error("Rollback to previous stable version.")

		currentTime = time.time()
		timeDiffSinceLastFailure = currentTime - lastFailure
		if timeDiffSinceLastFailure > 3600 * 2:
			Logger.error("Retrying...")
		else:
			Logger.error("Retrying in 1h...")
			time.sleep(3600)
		lastFailure = currentTime

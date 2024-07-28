import pathlib
import typing
import argparse
import sys
import time
import threading
import stat
import enum

from apps.bootloader.config import endpoint, uid
from apps.artifacts.plugins.fs.release.python.release import Release, Update
from apps.artifacts.plugins.fs.release.python.mock import ReleaseMock
from apps.bootloader.manifest import Manifest
from apps.bootloader.singleton import Singleton
from apps.bootloader.mutex import Mutex
from bzd.utils.logging import Logger
from bzd.utils.run import localCommand


class StablePolicy(enum.Enum):
	returnCodeZero = enum.auto()
	runningPast10s = enum.auto()
	runningPast1h = enum.auto()


class Bootloader:

	def __init__(self, root: pathlib.Path) -> None:
		self.root = root
		self.manifest = Manifest(root / "manifest.json")
		self.mutex = Mutex(root / "mutex.lock")
		self.release = Release(endpoint=endpoint, uid=uid)

	@staticmethod
	def _getBinaryDirectory(uid: str) -> pathlib.Path:
		return pathlib.Path("bin") / uid

	def getBinary(self, app: str, uid: str) -> pathlib.Path:
		"""Get the binary to run.

		This function checks for the validity of the binary and raise an exception on error.
		"""

		with self.mutex:
			maybeBinary = self.manifest.getBinary(uid)

			# If there is no binary, download one.
			if maybeBinary is None:
				maybeBinary = self._update(app=app, uid=uid)
				if maybeBinary is None:
					raise Exception("There is no binary, nor update available.")

			assert maybeBinary is not None, "Binary must exist."
			binary = self.root / maybeBinary
			if not binary.is_file():
				self.manifest.setBinary(uid, None)
				raise Exception(f"The binary '{binary}' must point to an existing file.")

			# Set execution permission.
			permissions = binary.stat().st_mode
			binary.chmod(permissions | stat.S_IEXEC)
			permissionsUpdated = binary.stat().st_mode
			assert permissionsUpdated & stat.S_IEXEC, f"Binary '{binary}' permissions couldn't be set to +x: {permissionsUpdated}"

			return binary

	def rollback(self, uid: str) -> bool:
		"""Rollback to the previous stable version."""

		with self.mutex:
			maybeStable = self.manifest.getStableBinary(uid)
			self.manifest.setBinary(uid, maybeStable)
			return True if maybeStable else False

	def clean(self, uid: str) -> None:
		"""Clean the cache for a specific UID."""

		with self.mutex:
			self.manifest.clean(uid)

	def update(self, app: str, uid: str) -> typing.Optional[pathlib.Path]:
		"""Try to update the binary and return the relative binary path if any."""

		with self.mutex:
			return self._update(app=app, uid=uid)

	def _update(self, app: str, uid: str) -> typing.Optional[pathlib.Path]:
		"""Try to update the binary and return the relative binary path if any."""

		# Perform the update
		maybeLastUpdate = self.manifest.getLastUpdate(uid)
		maybeUpdate = self.release.fetch(app=app, after=maybeLastUpdate.name if maybeLastUpdate else None)
		if not maybeUpdate:
			return None

		# Set the binary.
		path = Bootloader._getBinaryDirectory(uid=uid) / maybeUpdate.name
		fullPath = self.root / path
		maybeUpdate.toFile(fullPath)
		self.manifest.setBinary(uid, path)
		self.manifest.setUpdate(uid, path, int(time.time()))

		# Cleanup the bin folder, keep only the stable binary and this one.
		maybeStable = self.manifest.getStableBinary(uid)
		keep = {maybeStable.name, path.name} if maybeStable else {path.name}
		for f in fullPath.parent.iterdir():
			if f.name not in keep:
				try:
					f.unlink()
				except Exception as e:
					Logger.error(f"Cannot delete file {f}")

		return path

	def run(self,
	        app: str,
	        uid: str,
	        args: typing.Optional[typing.List[str]] = None,
	        stablePolicy: typing.Optional[StablePolicy] = None) -> None:
		"""Run the binary."""

		def markBinaryAsStable():
			with self.mutex:
				self.manifest.setStable(uid)

		binary = self.getBinary(app, uid)
		Logger.info(f"Running {binary} {' '.join(args or [])}")

		# Timer to set the binary as stable.
		if stablePolicy == StablePolicy.runningPast10s:
			timer = threading.Timer(10, markBinaryAsStable)
		elif stablePolicy == StablePolicy.runningPast1h:
			timer = threading.Timer(3600, markBinaryAsStable)
		else:
			timer = None
		if timer:
			timer.start()

		try:
			localCommand(cmds=[binary] + (args or []), timeoutS=None, stdout=True, stderr=True)
			if stablePolicy == StablePolicy.returnCodeZero:
				markBinaryAsStable()
		finally:
			if timer:
				timer.cancel()


class BootloaderForTest(Bootloader):

	def __init__(self, root: pathlib.Path, data: typing.Any) -> None:
		super().__init__(root)
		self.release = ReleaseMock(uid=uid, data=data)


def selfTests(cache) -> None:
	"""Perform a sequence of self tests."""

	app = "test"
	uid = "bootloader_self_test"

	bootloader = BootloaderForTest(
	    cache, {
	        app: [
	            {
	                "name": "first",
	                "binary": b"#!/usr/bin/env bash\nexit 0"
	            },
	            {
	                "name": "second",
	                "binary": b"#!/usr/bin/env bash\nexit 0"
	            },
	            {
	                "name": "third",
	                "binary": b"#!/usr/bin/env bash\nexit 1"
	            },
	        ]
	    })
	bootloader.clean(uid=uid)

	def assertBinary(name: str) -> None:
		binaryName = bootloader.getBinary(app, uid).name
		assert binaryName == name, f"Wrong binary expected, received '{binaryName}' instead of '{name}'"

	def assertUpdate(maybeUpdate: typing.Optional[pathlib.Path], name: typing.Optional[str]) -> None:
		assert (maybeUpdate is not None) or (name is None), f"Expected '{name}' but got no update"
		assert (name is not None) or (maybeUpdate is None), f"Got '{maybeUpdate.name}' but expected no update"
		assert (name is None) or (maybeUpdate.name == name), f"Expected '{name}' but got instead '{maybeUpdate.name}'"

	bootloader.run(app=app, uid=uid)
	assertBinary("first")
	assertUpdate(bootloader.update(app=app, uid=uid), "second")
	bootloader.run(app=app, uid=uid, stablePolicy=StablePolicy.returnCodeZero)
	assertBinary("second")
	assertUpdate(bootloader.update(app=app, uid=uid), "third")
	hasFailed = False
	try:
		bootloader.run(app=app, uid=uid)
	except:
		hasFailed = True
	assertBinary("third")
	assert hasFailed, f"The last binary must have failed."
	bootloader.rollback(uid=uid)
	bootloader.run(app=app, uid=uid)
	assertBinary("second")
	assertUpdate(bootloader.update(app=app, uid=uid), None)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bootloader.")
	parser.add_argument("--cache",
	                    type=pathlib.Path,
	                    default=pathlib.Path(__file__).parent / f".{pathlib.Path(__file__).name}",
	                    help="Cache directory.")
	parser.add_argument("app", type=str, help="The identifier of the application.")
	parser.add_argument("uid", type=str, help="The unique identifier of this instance.")

	args = parser.parse_args()
	assert args.uid != "bootloader", f"The UID cannot be '{args.uid}'."

	# Make sure only one instance of the bootloader is running at a time.
	#singleton = Singleton(args.cache / "singleton.lock")
	#if not singleton.lock():
	#	sys.exit(0)

	selfTests(args.cache)

	sys.exit(0)

	bootloader = Bootloader(args.cache)

	lastFailure = 0
	while True:
		try:
			bootloader.run(app=args.app, uid=args.uid, args=[], stablePolicy=StablePolicy.runningPast1h)
			sys.exit(0)
		except KeyboardInterrupt:
			Logger.info("<<< Keyboard interrupt >>>")
			sys.exit(130)
		except Exception as e:
			Logger.error(e)
			if bootloader.rollback(args.uid):
				Logger.error("Rollback to previous stable version.")

		currentTime = time.time()
		timeDiffSinceLastFailure = currentTime - lastFailure
		if timeDiffSinceLastFailure > 3600 * 2:
			Logger.error("Retrying immediately.")
		else:
			Logger.error("Retrying in 1h.")
			time.sleep(3600)
		lastFailure = currentTime

import pathlib
import typing
import time
import threading
import stat
import enum

from apps.artifacts.api.python.release.release import Release
from apps.artifacts.api.python.release.mock import ReleaseMock
from apps.bootloader.manifest import Manifest
from apps.bootloader.mutex import Mutex
from bzd.utils.logging import Logger
from bzd.utils.run import localCommand, Cancellation


class StablePolicy(enum.Enum):
	returnCodeZero = enum.auto()
	runningPast10s = enum.auto()
	runningPast1h = enum.auto()


class ExceptionBinaryAbort(Exception):
	pass


class Binary:

	def __init__(self, uid: str, app: str, root: pathlib.Path) -> None:
		self.uid = uid
		self.app = app
		self.root = root
		self.manifest = Manifest(root / "manifest.json")
		self.mutex = Mutex(root / "mutex.lock", timeoutS=3600)
		self.release = Release()
		self.cancellation = Cancellation()
		self.binaryAvailableEvent = threading.Event()

		Logger.info(f"Binary root for {self.uid} at {self.root}")

	def _getDirectory(self) -> pathlib.Path:
		return pathlib.Path("bin") / self.uid

	def get(self) -> typing.Optional[pathlib.Path]:
		"""Get the path of the binary to run.

		This function checks for the validity of the binary and raise an exception on error.
		"""

		with self.mutex:
			maybeBinary = self.manifest.getBinary(self.uid)
			if maybeBinary is None:
				return None

			binary = self.root / maybeBinary
			if not binary.is_file():
				self.manifest.setBinary(self.uid, None)
				raise Exception(f"The binary '{binary}' must point to an existing file.")

			# Set execution permission.
			permissions = binary.stat().st_mode
			binary.chmod(permissions | stat.S_IEXEC)
			permissionsUpdated = binary.stat().st_mode
			assert permissionsUpdated & stat.S_IEXEC, f"Binary '{binary}' permissions couldn't be set to +x: {permissionsUpdated}"

			return binary

	def wait(self) -> None:
		"""Wait for a binary to be available."""

		# Check if there is a binary, if so do not wait.
		with self.mutex:
			maybeBinary = self.manifest.getBinary(self.uid)
			if maybeBinary:
				return
			self.binaryAvailableEvent.clear()

		Logger.info(f"Waiting for binary '{self.app}' to be available...")
		self.binaryAvailableEvent.wait()

	def rollback(self) -> bool:
		"""Rollback to the previous stable version."""

		with self.mutex:
			maybeStable = self.manifest.getStableBinary(self.uid)
			if self.manifest.setBinary(self.uid, maybeStable):
				Logger.info(f"Rollback to {maybeStable}")
				return True
			return False

	def clean(self) -> None:
		"""Clean the cache for a specific UID."""

		with self.mutex:
			Logger.info(f"Cleanup {self.uid}")
			self.manifest.clean(self.uid)

	def update(self) -> typing.Optional[pathlib.Path]:
		"""Try to update the binary and return the relative binary path if any."""

		with self.mutex:

			# Perform the update.
			maybeLastUpdate = self.manifest.getLastUpdate(self.uid)

		# Check for updates, this can take up to several minutes.
		maybeUpdate = self.release.fetch(path=self.app,
		                                 uid=self.uid,
		                                 after=maybeLastUpdate.name if maybeLastUpdate else None)
		if not maybeUpdate:
			return None

		with self.mutex:

			# A concurrent update was made.
			maybeLastUpdateAfter = self.manifest.getLastUpdate(self.uid)
			if maybeLastUpdate != maybeLastUpdateAfter:
				return None

			# Set the binary.
			path = self._getDirectory() / maybeUpdate.name
			Logger.info(f"Updating {path}")
			fullPath = self.root / path
			maybeUpdate.toFile(fullPath)
			self.manifest.setBinary(self.uid, path)
			self.manifest.setUpdate(self.uid, path, int(time.time()))

			# Cleanup the bin folder, keep only the stable binary and this one.
			maybeStable = self.manifest.getStableBinary(self.uid)
			keep = {maybeStable.name, path.name} if maybeStable else {path.name}
			for f in fullPath.parent.iterdir():
				if f.name not in keep:
					try:
						f.unlink()
					except Exception as e:
						Logger.error(f"Cannot delete file {f}")

			# Notify that a binary is available.
			self.binaryAvailableEvent.set()

			return path

	def run(self,
	        args: typing.Optional[typing.List[str]] = None,
	        stablePolicy: typing.Optional[StablePolicy] = None) -> None:
		"""Run the binary."""

		binary = self.get()
		assert binary is not None, "Binary must exist."
		Logger.info(f"Running {binary} {' '.join(args or [])}")

		def markBinaryAsStable():
			with self.mutex:
				if self.manifest.setStable(self.uid):
					Logger.info(f"Marked {binary} as a stable binary.")

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
			localCommand(cmds=[binary] + (args or []),
			             timeoutS=None,
			             stdout=True,
			             stderr=True,
			             cancellation=self.cancellation)
			if stablePolicy == StablePolicy.returnCodeZero:
				markBinaryAsStable()
		except Exception as e:
			if self.cancellation.triggered:
				raise ExceptionBinaryAbort()
			raise
		finally:
			self.cancellation.reset()
			if timer:
				timer.cancel()

	def abort(self) -> None:
		"""Abort the current running binary if any."""

		self.cancellation.cancel()


class BinaryForTest(Binary):

	def __init__(self, uid: str, app: str, root: pathlib.Path, data: typing.Any) -> None:
		super().__init__(uid=uid, app=app, root=root)
		self.release = ReleaseMock(data=data)

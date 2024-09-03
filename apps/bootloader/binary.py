import pathlib
import typing
import time
import threading
import stat
import enum

from apps.artifacts.api.python.release.release import Release
from bzd.utils.logging import Logger
from bzd.utils.run import localCommand, Cancellation


class StablePolicy(enum.Enum):
	returnCodeZero = "returnCodeZero"
	runningPast10s = "runningPast10s"
	runningPast1h = "runningPast1h"


class ExceptionBinaryAbort(Exception):
	pass


class Binary:

	def __init__(self, binary: pathlib.Path) -> None:
		self.binary = Binary._validateBinaryPath(binary)
		self.cancellation = Cancellation()
		self.binaryAvailableEvent = threading.Event()

	@staticmethod
	def _validateBinaryPath(path: pathlib.Path) -> pathlib.Path:
		"""Get the path of a binary to run.

		This function checks for the validity of the binary and raise an exception on error.
		"""

		if not path.is_file():
			raise Exception(f"The path '{path}' must point to an existing file.")

		# Set execution permission.
		permissions = path.stat().st_mode
		path.chmod(permissions | stat.S_IEXEC)
		permissionsUpdated = path.stat().st_mode
		assert permissionsUpdated & stat.S_IEXEC, f"Binary '{path}' permissions couldn't be set to +x: {permissionsUpdated}"

		return path

	def run(self, args: typing.Optional[typing.List[str]], stablePolicy: typing.Optional[StablePolicy],
	        stableCallback: typing.Callable[[], None]) -> None:
		"""Run the binary."""

		Logger.info(f"Running {self.binary} {' '.join(args or [])}")

		# Timer to set the binary as stable.
		if stablePolicy == StablePolicy.runningPast10s:
			timer = threading.Timer(10, stableCallback)
		elif stablePolicy == StablePolicy.runningPast1h:
			timer = threading.Timer(3600, stableCallback)
		else:
			timer = None
		if timer:
			timer.start()

		try:
			localCommand(cmds=[self.binary] + (args or []),
			             timeoutS=None,
			             stdout=True,
			             stderr=True,
			             cancellation=self.cancellation)
			if stablePolicy == StablePolicy.returnCodeZero:
				stableCallback()
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
		self.binaryAvailableEvent.set()

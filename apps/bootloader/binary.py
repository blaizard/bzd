import pathlib
import typing
import time
import threading
import stat
import enum
import io

from apps.artifacts.api.python.release.release import Release
from bzd.utils.logging import Logger
from bzd.utils.run import localCommand, Cancellation


class StablePolicy(str, enum.Enum):
	returnCodeZero = "returnCodeZero"
	runningPast10s = "runningPast10s"
	runningPast1h = "runningPast1h"


class ExceptionBinaryAbort(Exception):
	pass


class StdOutputToLogger(io.TextIOBase):
	"""Proxy the standard output to a logger.
	
	It also buffers the output to ensure the content is properly displayed.
	"""

	def __init__(self, logger: Logger, bufferTimeS: float = 1., maxBufferSize: int = 10000) -> None:
		super().__init__()
		self.buffer = ""
		self.bufferTimeS = bufferTimeS
		self.maxBufferSize = maxBufferSize
		self.logger = logger
		self.stopRequested = False
		self.timeEvent = threading.Event()
		self.worker = threading.Thread(target=self.collector)

	def collector(self) -> None:

		while not self.stopRequested:
			# If set is called, reset the timer.
			if self.timeEvent.wait(timeout=self.bufferTimeS):
				self.timeEvent.clear()
			# The timeout kicked and there are data in the buffer.
			elif len(self.buffer) > 0:
				self.flush()
			if len(self.buffer) > self.maxBufferSize:
				self.flush()
		self.flush()

	def write(self, text: str) -> None:
		self.buffer += text
		self.timeEvent.set()

	def __enter__(self) -> "StdOutputToLogger":
		self.worker.start()
		return self

	def __exit__(self, *_: typing.Any) -> None:
		self.stopRequested = True
		self.timeEvent.set()
		self.worker.join()

	def flush(self) -> None:
		self.logger.info(self.buffer.strip())
		self.buffer = ""


class Binary:

	def __init__(self, binary: pathlib.Path, logger: Logger) -> None:
		self.binary = Binary._validateBinaryPath(binary)
		self.cancellation = Cancellation()
		self.logger = logger

	@staticmethod
	def setPermissions(path: pathlib.Path) -> None:
		"""Set proper permission to a binary path."""

		# Set execution permission if needed.
		if path.stat().st_mode & stat.S_IEXEC == 0:
			path.chmod(path.stat().st_mode | stat.S_IEXEC)
			assert path.stat(
			).st_mode & stat.S_IEXEC, f"Binary '{path}' permissions couldn't be set to +x: {permissionsUpdated}"

	@staticmethod
	def _validateBinaryPath(path: pathlib.Path) -> pathlib.Path:
		"""Get the path of a binary to run.

		This function checks for the validity of the binary and raise an exception on error.
		"""

		if not path.is_file():
			raise Exception(f"The path '{path}' must point to an existing file.")

		Binary.setPermissions(path)

		return path

	def run(self,
	        args: typing.Optional[typing.List[str]],
	        stablePolicy: typing.Optional[StablePolicy] = None,
	        stableCallback: typing.Optional[typing.Callable[[], None]] = None) -> None:
		"""Run the binary."""

		self.logger.info(f"Running {self.binary} {' '.join(args or [])}")

		# Timer to set the binary as stable.
		if stableCallback and stablePolicy == StablePolicy.runningPast10s:
			timer = threading.Timer(10, stableCallback)
		elif stableCallback and stablePolicy == StablePolicy.runningPast1h:
			timer = threading.Timer(3600, stableCallback)
		else:
			timer = None
		if timer:
			timer.start()

		with StdOutputToLogger(self.logger) as stdOutput:
			try:
				localCommand(cmds=[self.binary] + (args or []),
				             timeoutS=None,
				             stdout=stdOutput,
				             stderr=stdOutput,
				             cancellation=self.cancellation)
				if stableCallback and stablePolicy == StablePolicy.returnCodeZero:
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

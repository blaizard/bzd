"""System wide mutex."""

import pathlib
import os
import fcntl
import time

from bzd.utils.logging import Logger


def _flockNonBlocking(lockFile):
	try:
		fcntl.flock(lockFile, fcntl.LOCK_EX | fcntl.LOCK_NB)
	except OSError:
		return False
	return True


class Mutex:

	def __init__(self, path: pathlib.Path, timeoutS: float = 3600) -> None:
		self.path = path
		self.timeoutS = timeoutS
		self.path.parent.mkdir(parents=True, exist_ok=True)

	def __enter__(self) -> None:
		"""Implementation of a mutex.
		
		This implementation uses a non-blocking variant of flock for performance reason.

		Note that initially a blocking variant was used but has some limitation for implementing
		the timeout. The timeout was using the signal library which is not allowed outside of the
		main thread, therefore this implementation raise the error: 'signal only works in main thread of the main interpreter'
		"""

		def createLockFileTimestamp():
			lockFile = os.open(self.path, os.O_WRONLY | os.O_CREAT)
			timestampEnd = time.time() + self.timeoutS
			return lockFile, timestampEnd

		lockFile, timestampEnd = createLockFileTimestamp()
		while not _flockNonBlocking(lockFile):
			time.sleep(0.1)
			if time.time() > timestampEnd:
				# Timeout was raised.
				Logger.error(f"The mutex {self.path} timed out after {self.timeoutS}s, resetting the lock.")
				os.close(lockFile)
				os.remove(self.path)
				lockFile, timestampEnd = createLockFileTimestamp()

		self.lockFile = lockFile

	def __exit__(self, *args, **kwargs) -> None:

		lockFile = self.lockFile
		fcntl.flock(lockFile, fcntl.LOCK_UN)
		os.close(lockFile)
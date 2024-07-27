import fcntl
import os
import pathlib


class Singleton:

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path

	def lock(self) -> bool:
		"""Detect if an an instance with the label is already running, globally
		at the operating system level.

		Using `os.open` ensures that the file pointer won't be closed
		by Python's garbage collector after the function's scope is exited.

		The lock will be released when the program exits, or could be
		released if the file pointer were closed.

		Returns:
			True if the lock succeeded, False otherwise.
		"""

		self.path.parent.mkdir(parents=True, exist_ok=True)
		lockFile = os.open(self.path, os.O_WRONLY | os.O_CREAT)
		locked = False
		try:
			fcntl.lockf(lockFile, fcntl.LOCK_EX | fcntl.LOCK_NB)
			locked = True
		except IOError:
			pass

		return locked

"""System wide mutex."""

import pathlib
import os

# From: https://stackoverflow.com/a/60214222/1107046
if os.name == "nt":
	import msvcrt

	def portableLock(lockFile):
		msvcrt.locking(lockFile, msvcrt.LK_LOCK, 1)

	def portableUnlock(lockFile):
		msvcrt.locking(lockFile, msvcrt.LK_UNLCK, 1)
else:
	import fcntl

	def portableLock(lockFile):
		fcntl.flock(lockFile, fcntl.LOCK_EX)

	def portableUnlock(lockFile):
		fcntl.flock(lockFile, fcntl.LOCK_UN)


class Mutex:

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path
		self.path.parent.mkdir(parents=True, exist_ok=True)

	def __enter__(self) -> None:
		self.lockFile = os.open(self.path, os.O_WRONLY | os.O_CREAT)
		portableLock(self.lockFile)

	def __exit__(self, *args, **kwargs) -> None:
		portableUnlock(self.lockFile)
		os.close(self.lockFile)

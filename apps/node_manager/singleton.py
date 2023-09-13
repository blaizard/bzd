import fcntl
import os
import sys


def instanceAlreadyRunning(name: str) -> bool:
	"""Detect if an an instance with the label is already running, globally
    at the operating system level.

    Using `os.open` ensures that the file pointer won't be closed
    by Python's garbage collector after the function's scope is exited.

    The lock will be released when the program exits, or could be
    released if the file pointer were closed.
    """

	lockFile = os.open(f"/tmp/instance_{name}.lock", os.O_WRONLY | os.O_CREAT)

	try:
		fcntl.lockf(lockFile, fcntl.LOCK_EX | fcntl.LOCK_NB)
		alreadyRunning = False
	except IOError:
		alreadyRunning = True

	return alreadyRunning

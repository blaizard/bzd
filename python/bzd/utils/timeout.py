import typing
import threading


class TimeoutError(Exception):
	pass


class Timeout:
	"""Context manager to add a timeout to a block of code.
	
	Note that we cannot use `signal' here as it will not be supported
	with multithreading. It works only in the main thread.
	"""

	def __init__(self, seconds: int) -> None:
		self.seconds = seconds

	def __enter__(self) -> "Timeout":

		def timeout() -> None:
			raise TimeoutError(f"Timed out after {self.seconds}s.")

		self.timer = threading.Timer(self.seconds, timeout)
		self.timer.start()

		return self

	def __exit__(self, *args: typing.Any) -> None:
		self.timer.cancel()

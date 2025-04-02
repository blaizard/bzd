import signal
import typing


class TimeoutError(Exception):
	pass


class Timeout:
	"""Context manager to add a timeout to a block of code."""

	def __init__(self, seconds: int) -> None:
		self.seconds = seconds

	def __enter__(self) -> "Timeout":

		def signal_handler(*args: typing.Any) -> None:
			raise TimeoutError(f"Timed out after {self.seconds}s.")

		signal.signal(signal.SIGALRM, signal_handler)
		signal.alarm(self.seconds)
		return self

	def __exit__(self, *args: typing.Any) -> None:
		signal.alarm(0)

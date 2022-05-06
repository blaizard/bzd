import typing
import pathlib
import time


class Watcher:

	def __init__(self, path: pathlib.Path, callback: typing.Callable[[pathlib.Path], None], rateS: float = 1.) -> None:
		self.path = path
		self.callback = callback
		self.rateS = rateS
		self.stamp: float = 0.

	def poll(self) -> None:
		stamp = self.path.stat().st_mtime
		if stamp != self.stamp:
			self.stamp = stamp
			self.callback(self.path)

	def watch(self) -> None:
		while True:
			self.poll()
			time.sleep(self.rateS)

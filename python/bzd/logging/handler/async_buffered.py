import threading
import typing

from bzd.logging.handler import Log, LoggerHandlerScope, LoggerHandlerData, LoggerHandlerFlow


class LoggerHandlerAsyncBuffered(LoggerHandlerScope):
	"""Buffers the log entries."""

	def __init__(self, bufferTimeS: float = 1., maxBufferSize: int = 10000) -> None:
		self.data: typing.List[Log] = []
		self.size = 0
		self.flow: typing.Optional[LoggerHandlerFlow] = None
		self.bufferTimeS = bufferTimeS
		self.maxBufferSize = maxBufferSize
		self.stopRequested = False
		self.timeEvent = threading.Event()
		self.worker: typing.Optional[threading.Thread] = None

	def handler(self, data: LoggerHandlerData, flow: LoggerHandlerFlow) -> None:
		self.data += data
		self.size += sum([len(log.message) for log in data])
		self.flow = flow
		self.timeEvent.set()

	def collector(self) -> None:
		while not self.stopRequested:
			# If set is called, reset the timer.
			if self.timeEvent.wait(timeout=self.bufferTimeS):
				self.timeEvent.clear()
			# The timeout kicked and there are data in the buffer.
			elif self.size > 0:
				self.flush()
			if self.size > self.maxBufferSize:
				self.flush()
		self.flush()

	def flush(self) -> None:
		if self.flow and len(self.data):
			self.flow.next([*self.data])
			self.data = []
			self.size = 0

	def constructor(self, name: str) -> None:
		self.worker = threading.Thread(target=self.collector)
		self.worker.start()

	def destructor(self) -> None:
		self.stopRequested = True
		self.timeEvent.set()
		assert self.worker
		self.worker.join()

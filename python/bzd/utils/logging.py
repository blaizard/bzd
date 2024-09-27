import logging
import typing
import dataclasses
import time


@dataclasses.dataclass
class Log:
	# The timestamp of the log entry.
	timestamp: float
	# The message from the log.
	message: str


HandlerData = typing.List[Log]
HandlerResult = typing.Optional[HandlerData]

# Default logger
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] [%(levelname)s] [%(name)s] [%(filename)s:%(lineno)d] %(message)s",
)


class _CallbackHandler(logging.Handler):

	def __init__(self, callback: typing.Callable[[Log], None]) -> None:
		super().__init__()
		self.callback = callback

	def emit(self, record: typing.Any) -> None:
		msg = self.format(record).strip()
		if msg:
			self.callback(Log(timestamp=time.time(), message=msg))


class LoggerHandler:

	def handler(self, data: HandlerData) -> HandlerResult:
		raise Exception("Not implemented")


class LoggerHandlerStub(LoggerHandler):
	"""Stub that outputs nothing."""

	def handler(self, data: HandlerData) -> HandlerResult:
		return None


class LoggerHandlerInMemory(LoggerHandler):
	"""Log the content in-memory."""

	def __init__(self, maxBufferSize: int = 10000) -> None:
		self.maxBufferSize = maxBufferSize
		self.size = 0
		self.records: typing.List[Log] = []

	def handler(self, data: HandlerData) -> HandlerResult:

		for log in data:
			self.size += len(log.message)
			self.records.append(log)

		# Resize the buffer.
		while self.size > self.maxBufferSize:
			sizeToBeRemoved = self.size - self.maxBufferSize
			sizeNextEntry = len(self.records[0].message)
			sizeRemove = min(sizeNextEntry, sizeToBeRemoved)
			self.size -= sizeRemove
			self.records[0].message = self.records[0].message[sizeRemove:]
			if len(self.records[0].message) == 0:
				self.records.pop(0)

		return data

	def __iter__(self) -> typing.Iterator[Log]:
		"""Accessor for the in-memory data."""

		for log in self.records:
			yield log


class Logger:

	def __init__(self, name: str) -> None:
		self.logger = logging.getLogger(name)
		self._handlers: typing.List[LoggerHandler] = []

	def handlers(self, *handlers: LoggerHandler) -> "Logger":
		"""Register handlers to the current logger, this action disables the default backend."""

		self.logger.propagate = False
		self.logger.addHandler(_CallbackHandler(self._callback))
		self._handlers += handlers

		return self

	def _callback(self, log: Log) -> None:
		"""Calls all handlers sequentially."""

		data = [log]
		for handler in self._handlers:
			maybeData = handler.handler(data)
			if maybeData is None:
				break
			data = maybeData

	@property
	def info(self) -> typing.Callable[[str], None]:
		return self.logger.info

	@property
	def warning(self) -> typing.Callable[[str], None]:
		return self.logger.warning

	@property
	def error(self) -> typing.Callable[[str], None]:
		return self.logger.error

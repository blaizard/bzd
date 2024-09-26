import logging
import typing

# Default logger
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] [%(levelname)s] [%(name)s] [%(filename)s:%(lineno)d] %(message)s",
)


class _CallbackHandler(logging.Handler):

	def __init__(self, callback: typing.Callable[[str], None]) -> None:
		super().__init__()
		self.callback = callback

	def emit(self, record: typing.Any) -> None:
		msg = self.format(record).strip()
		if msg:
			self.callback(msg)


class LoggerBackend:

	def install(self, logger: logging.Logger) -> None:
		pass


class LoggerBackendStub(LoggerBackend):
	"""Stub that does nothing."""

	def install(self, logger: logging.Logger) -> None:
		logger.addHandler(logging.NullHandler())


class LoggerBackendInMemory(LoggerBackend):
	"""Log the content in-memory."""

	def __init__(self, maxBufferSize: int = 10000) -> None:
		self.maxBufferSize = maxBufferSize
		self.size = 0
		self.records: typing.List[typing.Tuple[int, str]] = []

	def install(self, logger: logging.Logger) -> None:
		logger.addHandler(_CallbackHandler(self._log))

	def _log(self, message: str) -> None:
		self.size += len(message)
		self.records.append((
		    len(message),
		    message,
		))

		# Resize the buffer.
		while self.size > self.maxBufferSize:
			sizeToBeRemoved = self.size - self.maxBufferSize
			sizeNextEntry = self.records[0][0]
			sizeRemove = min(sizeNextEntry, sizeToBeRemoved)
			self.size -= sizeRemove
			self.records[0] = (
			    sizeNextEntry - sizeRemove,
			    self.records[0][1][sizeRemove:],
			)
			if len(self.records[0][1]) == 0:
				self.records.pop(0)

	def data(self) -> typing.List[str]:
		"""Accessor for the in-memory data."""

		return [record[1] for record in self.records]


class LoggerBackendBuffered(LoggerBackend):
	"""Buffer the logs in memory before calling a callback."""

	def __init__(self, bufferTimeS: float = 1., maxBufferSize: int = 10000) -> None:
		pass


class Logger:

	def __init__(self, name: str) -> None:
		self.logger = logging.getLogger(name)

	def backend(self, *backends: LoggerBackend) -> "Logger":
		"""Set one of more bachends to this logger, disabling the default backend."""

		self.logger.propagate = False
		for backend in backends:
			backend.install(logger=self.logger)

		return self

	@property
	def info(self) -> typing.Callable[[str], None]:
		return self.logger.info

	@property
	def warning(self) -> typing.Callable[[str], None]:
		return self.logger.warning

	@property
	def error(self) -> typing.Callable[[str], None]:
		return self.logger.error

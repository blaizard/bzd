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
		msg = self.format(record)
		self.callback(msg)


class LoggerBackend:

	def install(self, logger: logging.Logger) -> None:
		pass


class LoggerBackendStub(LoggerBackend):
	"""Stub that does nothing."""

	def install(self, logger: logging.Logger) -> None:
		logger.addHandler(logging.NullHandler())


class LoggerBackendInMemory(LoggerBackend):
	"""Log the content in-memory.
	
	The logs are then available via the 'data' attribute.
	"""

	def __init__(self, name: str, maxLogs: int = 100) -> None:
		self.maxLogs = maxLogs
		self.data: typing.List[str] = []

	def install(self, logger: logging.Logger) -> None:
		logger.addHandler(_CallbackHandler(self._log))

	def _log(self, message: str) -> None:
		self.data.append(message)
		self.data = self.data[-self.maxLogs:]


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


class BufferLogger(Logger):
	"""Buffer the logs in memory before being emitted."""

	def __init__(self, name: str, bufferTimeS: float = 1., maxBufferSize: int = 10000) -> None:
		super().__init__(name)

import logging
import typing

# Default logger
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] [%(levelname)s] [%(name)s] [%(filename)s:%(lineno)d] %(message)s",
)


class Logger:

	def __init__(self, name: str) -> None:
		self.logger = logging.getLogger(name)

	@property
	def info(self) -> typing.Callable[[str], None]:
		return self.logger.info

	@property
	def warning(self) -> typing.Callable[[str], None]:
		return self.logger.warning

	@property
	def error(self) -> typing.Callable[[str], None]:
		return self.logger.error


class StubLogger(Logger):

	def __init__(self, name: str, maxLogs: int = 100) -> None:
		super().__init__(name)
		self.logger.propagate = False
		self.logger.addHandler(logging.NullHandler())


class _CallbackHandler(logging.Handler):

	def __init__(self, callback: typing.Callable[[str], None]) -> None:
		super().__init__()
		self.callback = callback

	def emit(self, record: typing.Any) -> None:
		msg = self.format(record)
		self.callback(msg)


class InMemoryLogger(Logger):
	"""Log the content in-memory.
	
	The logs are then available via the 'data' attribute.
	"""

	def __init__(self, name: str, maxLogs: int = 100) -> None:
		super().__init__(name)
		self.maxLogs = maxLogs
		self.logger.propagate = False
		self.logger.addHandler(_CallbackHandler(self._log))
		self.data: typing.List[str] = []

	def _log(self, message: str) -> None:
		self.data.append(message)
		self.data = self.data[-self.maxLogs:]


class BufferLogger(Logger):
	"""Buffer the logs in memory before being emitted."""

	def __init__(self, name: str, bufferTimeS: float = 1., maxBufferSize: int = 10000) -> None:
		super().__init__(name)

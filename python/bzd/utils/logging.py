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
	def info(self):
		return self.logger.info

	@property
	def warning(self):
		return self.logger.warning

	@property
	def error(self):
		return self.logger.error


class StubLogger(Logger):

	def __init__(self, name: str, maxLogs: int = 100) -> None:
		super().__init__(name)
		self.logger.propagate = False
		self.logger.addHandler(logging.NullHandler())


class CallbackHandler(logging.Handler):

	def __init__(self, callback: typing.Callable[[str], None]) -> None:
		super().__init__()
		self.callback = callback

	def emit(self, record) -> None:
		msg = self.format(record)
		self.callback(msg)


class InMemoryLogger(Logger):

	def __init__(self, name: str, maxLogs: int = 100) -> None:
		super().__init__(name)
		self.maxLogs = maxLogs
		self.logger.propagate = False
		self.logger.addHandler(CallbackHandler(self.log))
		self.data: typing.List[str] = []

	def log(self, message: str) -> None:
		self.data.append(message)
		self.data = self.data[-self.maxLogs:]

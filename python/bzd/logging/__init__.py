import logging
import typing
import time

from bzd.logging.handler import Log, LoggerHandler

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

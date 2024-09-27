import logging
import typing
import time
from contextlib import contextmanager

from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerScope, LoggerHandlerFlow, LoggerHandlerCallback

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
		self._handlers: typing.List[typing.List[LoggerHandlerCallback]] = []
		self.logger.addHandler(_CallbackHandler(self._callback))

	def handlers(self, *handlers: LoggerHandler) -> "Logger":
		"""Register handlers to the current logger, this action disables the default backend."""

		self.logger.propagate = False
		self._handlers.append([handler.handler for handler in handlers])

		return self

	@contextmanager
	def handlersScope(self, *handlers: typing.Union[LoggerHandler,
	                                                LoggerHandlerScope]) -> typing.Generator[None, None, None]:
		"""Register scoped handlers."""

		for handler in handlers:
			if isinstance(handler, LoggerHandlerScope):
				handler.constructor()

		index = len(self._handlers)
		self.logger.propagate = False
		self._handlers.append([handler.handler for handler in handlers])

		yield

		self._handlers.pop(index)

		for handler in handlers:
			if isinstance(handler, LoggerHandlerScope):
				handler.destructor()

	def _callback(self, log: Log) -> None:
		"""Calls all handlers sequentially."""

		for handlers in self._handlers:
			flow = LoggerHandlerFlow([*handlers])
			flow.next(data=[log])

	@property
	def info(self) -> typing.Callable[[str], None]:
		return self.logger.info

	@property
	def warning(self) -> typing.Callable[[str], None]:
		return self.logger.warning

	@property
	def error(self) -> typing.Callable[[str], None]:
		return self.logger.error

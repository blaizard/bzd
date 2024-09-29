import logging
import typing
from contextlib import contextmanager

from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerScope, LoggerHandlerFlow, LoggerHandlerCallback
from bzd.logging.handler.stderr import LoggerHandlerStderr

# Default logger
logging.basicConfig(level=logging.INFO)
_defaultHandlers = [[LoggerHandlerStderr().handler]]


class _CallbackHandler(logging.Handler):

	def __init__(self, callback: typing.Callable[[Log], None]) -> None:
		super().__init__()
		self.callback = callback

	def emit(self, record: typing.Any) -> None:
		msg = self.format(record).strip()
		if msg:
			self.callback(
			    Log(name=record.name,
			        timestamp=record.created,
			        message=msg,
			        level=record.levelname.lower(),
			        filename=record.filename,
			        line=record.lineno))


class Logger:

	def __init__(self, name: str) -> None:
		self.name = name
		self.logger = logging.getLogger(name)
		self._handlers: typing.List[typing.List[LoggerHandlerCallback]] = []
		self.logger.propagate = False
		self.logger.addHandler(_CallbackHandler(self._callback))

	def handlers(self, *handlers: LoggerHandler) -> "Logger":
		"""Register handlers to the current logger, this action disables the default backend."""

		for handler in handlers:
			handler.constructor(name=self.name)

		self._handlers.append([handler.handler for handler in handlers])

		return self

	@contextmanager
	def handlersScope(self, *handlers: typing.Union[LoggerHandler,
	                                                LoggerHandlerScope]) -> typing.Generator[None, None, None]:
		"""Register scoped handlers."""

		for handler in handlers:
			handler.constructor(name=self.name)

		index = len(self._handlers)
		self._handlers.append([handler.handler for handler in handlers])

		yield

		self._handlers.pop(index)

		for handler in handlers:
			if isinstance(handler, LoggerHandlerScope):
				handler.destructor()

	def _callback(self, log: Log) -> None:
		"""Calls all handlers sequentially."""

		for handlers in self._handlers or _defaultHandlers:
			flow = LoggerHandlerFlow([*handlers])
			flow.next(data=[log])

	def fromLogs(self, *logs: Log) -> None:
		"""Process raw logs."""

		for log in logs:
			self._callback(log)

	def child(self, name: str) -> "Logger":
		"""Create a child logger.
		
		It inherits all the handlers and cannot set new ones.
		"""
		# To be implemented.
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

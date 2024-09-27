import logging
import typing
import dataclasses


@dataclasses.dataclass
class Log:
	# The name of the logger.
	name: str
	# The timestamp of the log entry.
	timestamp: float
	# The message from the log.
	message: str
	# The name of the level.
	level: str
	# Filename portion of the path.
	filename: str
	# Source line number where the logging call was issued.
	line: int


LoggerHandlerData = typing.List[Log]
LoggerHandlerCallback = typing.Callable[[LoggerHandlerData, "LoggerHandlerFlow"], None]


class LoggerHandler:

	def handler(self, data: LoggerHandlerData, flow: "LoggerHandlerFlow") -> None:
		raise NotImplementedError("Not implemented")


class LoggerHandlerScope:

	def handler(self, data: LoggerHandlerData, flow: "LoggerHandlerFlow") -> None:
		raise NotImplementedError("Not implemented")

	def constructor(self) -> None:
		pass

	def destructor(self) -> None:
		pass


class LoggerHandlerFlow:

	def __init__(self, handlers: typing.List[LoggerHandlerCallback]) -> None:
		self._handlers = handlers

	def next(self, data: LoggerHandlerData) -> None:
		if len(self._handlers) > 0:
			handler = self._handlers.pop(0)
			handler(data, self)

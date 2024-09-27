import logging
import typing
import dataclasses


@dataclasses.dataclass
class Log:
	# The timestamp of the log entry.
	timestamp: float
	# The message from the log.
	message: str


LoggerHandlerData = typing.List[Log]
LoggerHandlerCallback = typing.Callable[[LoggerHandlerData, "LoggerHandlerFlow"], None]


class LoggerHandler:

	def handler(self, data: LoggerHandlerData, flow: "LoggerHandlerFlow") -> None:
		raise Exception("Not implemented")


class LoggerHandlerScope:

	def handler(self, data: LoggerHandlerData, flow: "LoggerHandlerFlow") -> None:
		raise Exception("Not implemented")

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

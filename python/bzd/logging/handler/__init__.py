import logging
import typing
import dataclasses


@dataclasses.dataclass
class Log:
	# The timestamp of the log entry.
	timestamp: float
	# The message from the log.
	message: str


HandlerData = typing.List[Log]
HandlerResult = typing.Optional[HandlerData]


class LoggerHandler:

	def handler(self, data: HandlerData) -> HandlerResult:
		raise Exception("Not implemented")

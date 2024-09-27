from bzd.logging.handler import LoggerHandler, LoggerHandlerData, LoggerHandlerFlow


class LoggerHandlerStdout(LoggerHandler):
	"""Print to stdout."""

	def handler(self, data: LoggerHandlerData, flow: LoggerHandlerFlow) -> None:

		for log in data:
			print(log.message)

		flow.next(data=data)

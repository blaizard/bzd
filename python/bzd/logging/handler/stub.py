from bzd.logging.handler import LoggerHandler, HandlerData, HandlerResult


class LoggerHandlerStub(LoggerHandler):
	"""Stub that outputs nothing."""

	def handler(self, data: HandlerData) -> HandlerResult:
		return None
